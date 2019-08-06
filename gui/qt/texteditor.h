#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>

class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;

class TextEditor : public QMainWindow
{
  Q_OBJECT

public:
  TextEditor();

  void loadFile(const QString &fileName);

protected:
  void closeEvent(QCloseEvent *event) override;

  private slots:
  void newFile();
  void open();
  bool save();
  bool saveAs();
  void about();
  void documentWasModified();
#ifndef QT_NO_SESSIONMANAGER
  void commitData(QSessionManager &);
#endif

private:
  void createActions();
  void createStatusBar();
  void readSettings();
  void writeSettings();
  bool maybeSave();
  bool saveFile(const QString &fileName);
  void setCurrentFile(const QString &fileName);
  QString strippedName(const QString &fullFileName);

  QPlainTextEdit *textEdit;
  QString curFile;
};

#endif // TEXTEDITOR_H
