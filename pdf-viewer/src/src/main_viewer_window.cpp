#include "main_viewer_window.h"
#include "events/key_event.h"
//#include "open_dialog.hpp"
//#include "save_dialog.hpp"
//#include "text_file.hpp"
//#include "dialog.hpp"
#include "application.h"
//#include "full_file_name.hpp"
//#include "docpane/pdf_doc_mock.hpp"

#include "viewer/view.h"
#include "viewer/layout.h"
#include "viewer/document.h"
#include "viewer/params.h"
#include "globals.h"

#include <algorithm>
#include <cassert>
#include <iostream>

MainViewerWindow::MainViewerWindow(Widget *parent):
  Widget(parent),
  activeScreen_(nullptr),
  layout_(Layout::Vertical),
  //tabs_(this),
  screenLayout_(Layout::Vertical)
{
    //PdfDoc pdf_doc("first.pdf", 3);
    //PdfDoc pdf_doc("first.pdf", 9);
    //ViewerParams viewer_params;
    //activeScreen_ = new PdfScreen(this, viewer_params, pdf_doc);
    
    PdfViewer::Viewer::Params params;
    
    g_document = new PdfViewer::Viewer::Document("pointer.pdf");
    //PdfViewer::Viewer::Document doc("second.pdf");
    g_document->load_document();
    activeScreen_ = new PdfViewer::Viewer::View(this, params,
                                                PdfViewer::Viewer::LayoutType::Continuous);

  //using namespace std::placeholders;
  //tabs_.setTextBuffer = std::bind(&MainViewerWindow::setTextBuffer, this, _1);
  //tabs_.deleteTextBuffer = std::bind(&MainViewerWindow::deleteTextBuffer, this, _1);
  setLayout(&layout_);
  //layout_.addLayoutable(&tabs_);
  screenLayout_.addLayoutable(activeScreen_);
  layout_.addLayoutable(&screenLayout_);
  activeScreen_->setFocus();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

bool MainViewerWindow::keyPressEvent(KeyEvent &e)
{
  std::cout << __func__ << " key: " << e.key() << " modifiers: " << e.modifiers() << std::endl;
  bool result1 = true;
  if ((e.modifiers() & (KeyEvent::MLCtrl | KeyEvent::MRCtrl)) != 0)
  {
    switch (e.key())
    {
    case KeyEvent::K2:
      //wholeScreen();
      break;
    case KeyEvent::K3:
      //split(Layout::Vertical);
      break;
    case KeyEvent::K4:
      //split(Layout::Horizontal);
      break;
    case KeyEvent::KO:
      {
        break;
        //throw "not implemented";
 /*       auto &buffersList = tabs_.textBuffersList();
        auto tmp = std::find_if(std::begin(buffersList), std::end(buffersList),
                                [](BaseTextBuffer *x) 
                                { 
                                  return dynamic_cast<OpenDialog *>(x); 
                                });
        if (tmp == std::end(buffersList))
        {
          auto openDialog = new OpenDialog(activeScreen_);
          using namespace std::placeholders;
          openDialog->openFile = std::bind(&MainViewerWindow::openFile, this, _1, _2);
          tabs_.addTextBuffer(openDialog);
        }
        else
          tabs_.setActiveTextBuffer(*tmp);
        break;
 */
      }
    case KeyEvent::KPageUp:
    case KeyEvent::KLeft:
      //tabs_.switchToPrevTextBuffer();
      break;
    case KeyEvent::KPageDown:
    case KeyEvent::KRight:
      //tabs_.switchToNextTextBuffer();
      break;
    case KeyEvent::KTab:
      //switchToNextScreen();
      break;
    default:
      result1 = false;
    }
  }
  else if ((e.modifiers() & (KeyEvent::MLAlt | KeyEvent::MRAlt)) != 0)
  {
    switch (e.key())
    {
    case KeyEvent::KLeft:
      //switchToPrevScreen();
      break;
    case KeyEvent::KRight:
      //switchToNextScreen();
      break;
    default:
      result1 = false;
    }
  }
  else
    result1 = false;

  bool result2 = true;
  if ((e.modifiers() & (KeyEvent::MCtrl | KeyEvent::MShift)) != 0)
  {
    switch (e.key())
    {
    case KeyEvent::KLeft:
      //tabs_.moveTextBufferLeft();
      break;
    case KeyEvent::KRight:
      //tabs_.moveTextBufferRight();
      break;
    default:
      result2 = false;
    }
  }
  else
    result2 = false;

  return result1 || result2;
}

#pragma GCC diagnostic pop
