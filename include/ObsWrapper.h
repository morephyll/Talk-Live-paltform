#pragma once

#include "obs.h"
#include "obs.hpp"
#include <string>

using namespace std;

class ObsWrapper
{
public:
	ObsWrapper();
	~ObsWrapper();

	bool init_obs();
	int  start_rec();
	int  stop_rec();

private:
	bool ResetAudio();
	int ResetVideo();
	int set_scene_source();
	bool create_output_mode();
	void SetupFFmpeg();

private:
	OBSOutput fileOutput;
	obs_source_t* fadeTransition = nullptr;
	obs_scene_t* scene = nullptr;
	obs_source_t* captureSource;
	obs_properties_t* properties;

	OBSEncoder aacTrack[MAX_AUDIO_MIXES];
	std::string aacEncoderID[MAX_AUDIO_MIXES];
};

