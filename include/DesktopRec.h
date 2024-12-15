#pragma once

#include <QtWidgets/QWidget>
#include "ui_DesktopRec.h"
#include "ObsWrapper.h"
#include <memory>
#include <QTimer>
#include "CTitleBar.h";

enum SKinType
{
    WHITE_SKIN,
    DARK_SHIN
};

class DesktopRec : public QWidget
{
    Q_OBJECT

public:
    DesktopRec(QWidget *parent = Q_NULLPTR);

    void LoadSkin(QString qss_path);

    void paintEvent(QPaintEvent* event) override;

public slots:
    void on_btnRec_clicked();
    void on_timer();
    void on_show_shinmenu(const QPoint& p);
    
private:
    Ui::DesktopRecClass ui;

    bool m_bRec = false;

    std::unique_ptr<ObsWrapper> obs;
    QTimer* m_pTimer;
    int m_RecSeconds = 0; 
    CTitleBar* m_pTitle;
    SKinType m_SkinType = DARK_SHIN;
};
