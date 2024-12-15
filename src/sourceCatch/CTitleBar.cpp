#include "CTitleBar.h"
#include <QMouseEvent>
#include <qt_windows.h>
#include <QStyleOption> 
#include <QPainter>


#pragma comment(lib, "user32.lib")


CTitleBar::CTitleBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setStyleSheet("background-color:rgb(54,54,54);border-top-left-radius:15px;border-top-right-radius:15px;");

	ui.label_logo->setText("");
	ui.label_logo->setFixedSize(32, 32);
	QPixmap pix(":/DesktopRec/picture/rec_logo.svg");
	ui.label_logo->setPixmap(pix);

	ui.btnSkin->setText("");
	ui.btnSkin->setFixedSize(32, 32);

	ui.btnSet->setText("");
	ui.btnSet->setFixedSize(32, 32);

	ui.btnMin->setText("");
	ui.btnMin->setFixedSize(32, 32);

	ui.btnClose->setText("");
	ui.btnClose->setFixedSize(32, 32);

	setFixedHeight(32 * 1.5);
}

CTitleBar::~CTitleBar()
{
}

void CTitleBar::mousePressEvent(QMouseEvent* event)
{
	if (ReleaseCapture())
	{
		QWidget* pWin = this->window();
		if (pWin->isTopLevel())
		{
			SendMessage((HWND)(pWin->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, NULL);
		}
	}

	event->ignore();
}

void CTitleBar::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);

	QPainter p(this);

	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CTitleBar::on_btnSet_clicked()
{

}

void CTitleBar::on_btnMin_clicked()
{
	QWidget* pWin = this->window();
	pWin->showMinimized();
}

void CTitleBar::on_btnSkin_clicked()
{
	//获取控件的绝对位置
	QPoint ab_pos = ui.btnSkin->mapToGlobal(QPoint(0, 0));
	emit sig_ShowSkinMenu(ab_pos);
}

void CTitleBar::on_btnClose_clicked()
{
	QWidget* pWin = this->window();
	pWin->close();
}
