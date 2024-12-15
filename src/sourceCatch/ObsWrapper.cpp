#include "ObsWrapper.h"
#include <string>
#include <QApplication>
#include "libavcodec/avcodec.h"

using namespace std;


#define DL_D3D11 = "libobs-d3d11.dll"
#define DL_OPENGL = "libobs-opengl.dll"

#define INPUT_AUDIO_SOURCE "wasapi_input_capture"
#define OUTPUT_AUDIO_SOURCE "wasapi_output_capture"

#define VIDEO_ENCODER_ID           AV_CODEC_ID_H264
#define VIDEO_ENCODER_NAME         "libx264"
#define RECORD_OUTPUT_FORMAT       "mp4"
#define RECORD_OUTPUT_FORMAT_MIME  "video/mp4"
#define VIDEO_FPS            45
#define VIDEO_ENCODER_ID           AV_CODEC_ID_H264
#define AUDIO_BITRATE 128 
#define VIDEO_BITRATE 150 
#define OUT_WIDTH  1920
#define OUT_HEIGHT 1080

enum SOURCE_CHANNELS {
	SOURCE_CHANNEL_TRANSITION,
	SOURCE_CHANNEL_AUDIO_OUTPUT,
	SOURCE_CHANNEL_AUDIO_OUTPUT_2,
	SOURCE_CHANNEL_AUDIO_INPUT,
	SOURCE_CHANNEL_AUDIO_INPUT_2,
	SOURCE_CHANNEL_AUDIO_INPUT_3,
};

ObsWrapper::ObsWrapper()
{
}

ObsWrapper::~ObsWrapper()
{
}

bool ObsWrapper::init_obs()
{
	string cfg_path = "D:/desktop_rec_cfg";

	if (!obs_initialized())
	{
		//初始化obs
		if (!obs_startup("zh-CN", cfg_path.c_str(), NULL))
		{
			return false;
		}

		//加载插件
		QString path = qApp->applicationDirPath();
		string path_str = path.toStdString();

        string plugin_path = path_str + "/obs-plugins/64bit";
		string data_path = path_str + "/data/obs-plugins/%module%";

		obs_add_module_path(plugin_path.c_str(), data_path.c_str());

		obs_load_all_modules();
	}

	//音频设置
	if (!ResetAudio())
		return false;

	//视频设置
	if (ResetVideo() != OBS_VIDEO_SUCCESS)
		return false;

	if (0 != set_scene_source())
		return false;

	if (!create_output_mode())
		return false;

	return true;
}

int ObsWrapper::start_rec()
{
	SetupFFmpeg();

	if(!obs_output_start(fileOutput))
		return -1;

	return 0;
}

int ObsWrapper::stop_rec()
{
	bool force = true;

	if (force)
		obs_output_force_stop(fileOutput);
	else
		obs_output_stop(fileOutput);

	return 0;
}

bool ObsWrapper::ResetAudio()
{
	struct obs_audio_info ai;
	ai.samples_per_sec = 48000;
	ai.speakers = SPEAKERS_STEREO;

	return obs_reset_audio(&ai);
}

int ObsWrapper::ResetVideo()
{
	struct obs_video_info ovi;
	ovi.fps_num = VIDEO_FPS;
	ovi.fps_den = 1;

	ovi.graphics_module = "libobs-d3d11.dll";
	ovi.base_width = 1920;
	ovi.base_height = 1080;
	ovi.output_width = 1920;
	ovi.output_height = 1080;
	ovi.output_format = VIDEO_FORMAT_I420;
	ovi.colorspace = VIDEO_CS_709;
	ovi.range = VIDEO_RANGE_FULL;
	ovi.adapter = 0;
	ovi.gpu_conversion = true;
	ovi.scale_type = OBS_SCALE_BICUBIC;

	return obs_reset_video(&ovi);
}

void ResetAudioDevice(const char* sourceId, const char* deviceId,
	const char* deviceDesc, int channel)
{
	bool disable = deviceId && strcmp(deviceId, "disabled") == 0;
	obs_source_t* source;
	obs_data_t* settings;

	source = obs_get_output_source(channel);
	if (source) {
		if (disable) {
			obs_set_output_source(channel, nullptr);
		}
		else {
			settings = obs_source_get_settings(source);
			const char* oldId =
				obs_data_get_string(settings, "device_id");
			if (strcmp(oldId, deviceId) != 0) {
				obs_data_set_string(settings, "device_id",
					deviceId);
				obs_source_update(source, settings);
			}
			obs_data_release(settings);
		}

		obs_source_release(source);

	}
	else if (!disable) {
		settings = obs_data_create();
		obs_data_set_string(settings, "device_id", deviceId);
		source = obs_source_create(sourceId, deviceDesc, settings,
			nullptr);
		obs_data_release(settings);

		obs_set_output_source(channel, source);
		obs_source_release(source);
	}
}

static inline bool HasAudioDevices(const char* source_id)
{
	const char* output_id = source_id;
	obs_properties_t* props = obs_get_source_properties(output_id);
	size_t count = 0;

	if (!props)
		return false;

	obs_property_t* devices = obs_properties_get(props, "device_id");
	if (devices)
		count = obs_property_list_item_count(devices);

	obs_properties_destroy(props);

	return count != 0;
}

static void AddSource(void* _data, obs_scene_t* scene)
{
	obs_source_t* source = (obs_source_t*)_data;
	obs_scene_add(scene, source);
	obs_source_release(source);
}

int ObsWrapper::set_scene_source()
{
	obs_set_output_source(SOURCE_CHANNEL_TRANSITION, nullptr);
	obs_set_output_source(SOURCE_CHANNEL_AUDIO_OUTPUT, nullptr);
	obs_set_output_source(SOURCE_CHANNEL_AUDIO_INPUT, nullptr);

	size_t idx = 0;
	const char* id;

	/* automatically add transitions that have no configuration (things
	 * such as cut/fade/etc) */
	while (obs_enum_transition_types(idx++, &id)) {
		const char* name = obs_source_get_display_name(id);

		if (!obs_is_source_configurable(id)) {
			obs_source_t* tr = obs_source_create_private(id, name, NULL);
			
			if (strcmp(id, "fade_transition") == 0)
				fadeTransition = tr;
		}
	}

	if (!fadeTransition)
	{
		return -1;
	}

	obs_set_output_source(SOURCE_CHANNEL_TRANSITION, fadeTransition);
	obs_source_release(fadeTransition);

	scene = obs_scene_create("MyScene");
	if (!scene)
	{
		return -2;
	}

	obs_source_t* s = obs_get_output_source(SOURCE_CHANNEL_TRANSITION);
	obs_transition_set(s, obs_scene_get_source(scene));
	obs_source_release(s);

	bool hasDesktopAudio = HasAudioDevices(OUTPUT_AUDIO_SOURCE);
	bool hasInputAudio = HasAudioDevices(INPUT_AUDIO_SOURCE);

	if (hasDesktopAudio)
		ResetAudioDevice(OUTPUT_AUDIO_SOURCE, "default",
			"Default Desktop Audio", SOURCE_CHANNEL_AUDIO_OUTPUT);

	if (hasInputAudio)
		ResetAudioDevice(INPUT_AUDIO_SOURCE, "default",
			"Default Mic/Aux", SOURCE_CHANNEL_AUDIO_INPUT);

	//创建源：显示器采集
	captureSource = obs_source_create("monitor_capture", "Computer_Monitor_Capture", NULL, nullptr);
	//captureSource = obs_source_create("window_capture", "MyWindow_Capture", NULL, nullptr);

	if (captureSource) 
	{
		obs_scene_atomic_update(scene, AddSource, captureSource);
	}
	else 
	{
		return -3;
	}

	// 设置窗口捕获原的窗口或显示器
	obs_data_t* setting = obs_data_create();
	obs_data_t* curSetting = obs_source_get_settings(captureSource);
	obs_data_apply(setting, curSetting);
	obs_data_release(curSetting);

	properties = obs_source_properties(captureSource);

	obs_property_t* property = obs_properties_first(properties);

	string test_title = "[AAA.exe]: AAA";

	//BitBlt : gdi无法采集透明窗口
	
	while (property) 
	{
		const char* name = obs_property_name(property);
		if (strcmp(name, "window") == 0) 
		{
			size_t count = obs_property_list_item_count(property);
			const char* string = nullptr;

			for (size_t i = 0; i < count; i++) 
			{
				const char* item_name = obs_property_list_item_name(property, i);
				string = item_name;
				break;

				//窗口采集的设置
				/*if (strcmp(item_name, test_title.c_str()) == 0) 
				{
					string = obs_property_list_item_string(property, i);
					break;
				}*/
			}

			if (string)
			{
				obs_data_set_string(setting, name, string);
				obs_source_update(captureSource, setting);
				break;
			}
			else
			{
				obs_data_release(setting);

				return -4;
			}
		}

		obs_property_next(&property);
	}

	obs_data_release(setting);

	return 0;
}

static bool CreateAACEncoder(OBSEncoder& res, string& id,
	const char* name, size_t idx)
{
	const char* id_ = "ffmpeg_aac";

	res = obs_audio_encoder_create(id_, name, nullptr, idx, nullptr);

	if (res) {
		obs_encoder_release(res);
		return true;
	}

	return false;
}

bool ObsWrapper::create_output_mode()
{
	if (!fileOutput)
	{
		//高级输出 ffmpeg
		fileOutput = obs_output_create("ffmpeg_output", "adv_ffmpeg_output", nullptr, nullptr);

		if (!fileOutput)
			return false;
	}
	
	for (int i = 0; i < MAX_AUDIO_MIXES; i++) {
		char name[9];
		sprintf(name, "adv_aac%d", i);

		if (!CreateAACEncoder(aacTrack[i], aacEncoderID[i], name, i))
		{
			return false;
		}

		obs_encoder_set_audio(aacTrack[i], obs_get_audio());
	}

	return true;
}

/**
 * @brief 长整数转string, 主要是针对时间戳
 */
string i64_to_string(__int64 number)
{
	char str[20];  //足够了
	_i64toa(number, str, 10);
	string s(str);
	return s;
}

/**
 * @brief 产生时间秒数
 */
time_t getTimeSeconds()
{
	time_t myt = time(NULL);
	return myt;
}

/**
 * @brief 获取时间秒数并转为字符串
 */
string getTimeSecondsString()
{
	std::string str = i64_to_string(getTimeSeconds());
	return str;
}

void ObsWrapper::SetupFFmpeg()
{
	obs_data_t* settings = obs_data_create();

	string timestr = getTimeSecondsString();
	string out_file_name = "D:/" + timestr + ".mp4";

	obs_data_set_string(settings, "url", out_file_name.c_str());
	obs_data_set_string(settings, "format_name", RECORD_OUTPUT_FORMAT);
	obs_data_set_string(settings, "format_mime_type", RECORD_OUTPUT_FORMAT_MIME);
	obs_data_set_string(settings, "muxer_settings", "movflags=faststart"); 
	obs_data_set_int(settings, "gop_size", VIDEO_FPS * 10);
	obs_data_set_string(settings, "video_encoder", VIDEO_ENCODER_NAME);
	obs_data_set_int(settings, "video_encoder_id", VIDEO_ENCODER_ID);

	if (VIDEO_ENCODER_ID == AV_CODEC_ID_H264)
		obs_data_set_string(settings, "video_settings", "profile=main x264-params=crf=22");
	else if (VIDEO_ENCODER_ID == AV_CODEC_ID_FLV1)
		obs_data_set_int(settings, "video_bitrate", VIDEO_BITRATE);

	obs_data_set_int(settings, "audio_bitrate", AUDIO_BITRATE);
	obs_data_set_string(settings, "audio_encoder", "aac");
	obs_data_set_int(settings, "audio_encoder_id", AV_CODEC_ID_AAC);
	obs_data_set_string(settings, "audio_settings", NULL);

	obs_data_set_int(settings, "scale_width", OUT_WIDTH);
	obs_data_set_int(settings, "scale_height", OUT_HEIGHT);

	obs_output_set_mixer(fileOutput, 1);  //混流器，如果不设置，可能只有视频没有音频
	obs_output_set_media(fileOutput, obs_get_video(), obs_get_audio());
	obs_output_update(fileOutput, settings);

	obs_data_release(settings);
}

