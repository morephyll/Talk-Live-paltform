#include "DesktopRec.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QMenu>

static QString recStyle = "QPushButton#btnRec{border-radius:32px;background-image:url(:/DesktopRec/picture/rec.svg)}"
"QPushButton#btnRec:hover{border-radius:32px;background-image:url(:/DesktopRec/picture/rec_hover.svg)}";

static QString recingStyle = "QPushButton#btnRec{border-radius:32px;background-image:url(:/DesktopRec/picture/recing.svg)}"
"QPushButton#btnRec:hover{border-radius:32px;background-image:url(:/DesktopRec/picture/recing_hover.svg)}";

DesktopRec::DesktopRec(QWidget *parent)
    : QWidget(parent),
    obs(new ObsWrapper())
{
    ui.setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_pTimer = new QTimer(this);
    m_pTitle = new CTitleBar(this);

    QHBoxLayout* pHLay = new QHBoxLayout(this);
    m_pTitle->setFixedWidth(this->width());
    pHLay->addWidget(m_pTitle, 0, Qt::AlignTop);
    setLayout(pHLay);

    ui.btnRec->setText("");
    ui.btnRec->setStyleSheet(recStyle);

    LoadSkin(":/DesktopRec/picture/skin_dark.qss");

    if (!obs->init_obs())
    {
        QMessageBox::information(this, u8"warning", u8"obs init is not correctly");
        exit(0);
    }

    connect(m_pTimer, &QTimer::timeout, this, &DesktopRec::on_timer);
    connect(m_pTitle, &CTitleBar::sig_ShowSkinMenu, this, &DesktopRec::on_show_shinmenu);
}

void DesktopRec::on_btnRec_clicked()
{
    if (!m_bRec)
    {
        if (0 == obs->start_rec())
        {
            m_bRec = true;
        }
        else
        {
            QMessageBox::information(this, u8"warning", u8"obs capture is not starting");
        }

        ui.btnRec->setStyleSheet(recingStyle);
        m_pTimer->start(1000);
    }
    else
    {
        m_bRec = false;

        obs->stop_rec();
        m_pTimer->stop();

        ui.btnRec->setStyleSheet(recStyle);
        m_RecSeconds = 0;
    }
}

void DesktopRec::LoadSkin(QString qss_path)
{
    QFile file(qss_path);

    QString qss;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream text_stream(&file);
        while (!text_stream.atEnd())
        {
            qss += text_stream.readLine();
        }
    }

    file.close();
    this->setStyleSheet(qss);
}

void DesktopRec::on_timer()
{
    m_RecSeconds++;

    int hours = m_RecSeconds / 3600;
    int mins = (m_RecSeconds - hours * 3600) / 60;
    int secs = m_RecSeconds % 3600;

    char buf[256];
    memset(buf, 0, 256);

    sprintf(buf, "%04d:%02d:%02d", hours, mins, secs);

    ui.lcdNumber->display(buf);
}

//»»·ô²Ëµ¥
void DesktopRec::on_show_shinmenu(const QPoint& p)
{
    QMenu* pMenu = new QMenu(this);
    QAction* pAc1 = new QAction(u8"°×É«", this);
    QAction* pAc2 = new QAction(u8"°µºÚÉ«", this);

    connect(pAc1, &QAction::triggered, [=] {
        LoadSkin(":/DesktopRec/picture/skin_white.qss");
        m_SkinType = WHITE_SKIN;
    });

    connect(pAc2, &QAction::triggered, [=] {
        LoadSkin(":/DesktopRec/picture/skin_dark.qss");
        m_SkinType = DARK_SHIN;
    });

    pMenu->addAction(pAc1);
    pMenu->addAction(pAc2);

    int x = p.x();
    int y = p.y() + 36;
    pMenu->exec(QPoint(x, y));
}

void DesktopRec::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if(m_SkinType == DARK_SHIN)
        p.setBrush(QBrush(QColor(34, 34, 34)));
    else
        p.setBrush(QBrush(QColor(233, 233, 233)));

    p.setPen(Qt::transparent);

    QRect rect = this->rect();
    p.drawRoundedRect(rect, 15, 15);  
}
