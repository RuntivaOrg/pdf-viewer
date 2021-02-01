#pragma once
//#include "tabs.hpp"
//#include "pdf-screen.hpp"
#include "viewer/view.h"
//#include "status_bar.hpp"
#include "layout.h"
#include "widget.h"
//#include "dialog.hpp"
#include <string>

//class OpenDialog;
//class SaveDialog;
//class TextFile;

class MainViewerWindow : public Widget {
public:
    MainViewerWindow(Widget* parent = nullptr);

protected:
  virtual bool keyPressEvent(KeyEvent &)  override;
private:
  //PdfScreen *activeScreen_;
    PdfViewer::Viewer::View* activeScreen_;
    Layout layout_;
  //Tabs tabs_;
  Layout screenLayout_;

  //void openFile(OpenDialog *, const std::string &);
  //void saveAs(SaveDialog *sender, TextFile *textFile, const std::string &fileName);
  //void saveAndClose(SaveDialog *sender, TextFile *textFile, const std::string &fileName);
  //void closeActiveTextBuffer(Dialog::Answer);
  //void save();
  //void wholeScreen();
  void switchToPrevScreen();
  void switchToNextScreen();
  //void setTextBuffer(BaseTextBuffer *);
  //void deleteTextBuffer(BaseTextBuffer *);
};
