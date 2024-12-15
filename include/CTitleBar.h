#pragma once

#include <QWidget>
#include "ui_CTitleBar.h"

class CTitleBar : public QWidget
{
	Q_OBJECT

public:
	CTitleBar(QWidget *parent = Q_NULLPTR);
	~CTitleBar();

	void mousePressEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* e) override;

signals:
	void sig_ShowSkinMenu(const QPoint& p);

private slots:
	void on_btnSet_clicked();
	void on_btnMin_clicked();
	void on_btnSkin_clicked();
	void on_btnClose_clicked();

private:
	Ui::CTitleBar ui;
};
