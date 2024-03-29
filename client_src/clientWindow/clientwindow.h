#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QListView>
#include <QSpinBox>
#include "StringList.h"
#include "Resolution.h"
#include "ResolutionList.h"

#include "../Client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

// Ventana principal permite al usuario manejar el flujo del login.
class ClientWindow : public QMainWindow {
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private slots:
    void connectToServer();
    void joinMatchMenu();

    void createMatchMenu();
    void createMatch();
    void joinMatch();
    void joinCreated();

private:
    Ui::ClientWindow *ui;
    QWidget* loginPage;
    QWidget* optionPage;
    QWidget* newGamePage;
    QWidget* joinGamePage;
    QWidget *inGamePage;

    QStackedWidget* stack;
    QPushButton *loginButton;
    QPushButton *optionjoinButton;
    QPushButton *optionCreateButton;
    QLineEdit*  name;
    QLineEdit*  host;
    QSpinBox*   port;
    StringList mapList;
    StringList matchList;
    ResolutionList resoList;
    Resolution resolution;

    Client* client;
    ProtectedQueue<UI_Info> drawing_info;
    BlockingQueue<int> instructions;


    void linkToUI();
    void initWidgets();
    void connectEvents();
    void gameLoop();
    QPushButton *selectMatchButton;
    QPushButton *selectMapButton;


    QWidget *wantToJoin;
    QPushButton *playButton;
};

#endif // CLIENTWINDOW_H
