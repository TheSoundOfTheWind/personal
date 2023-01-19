#include <chrono>
#include <QMutex>
#include <iostream>
#include "enThread.h"
struct enThread::Data {
  Data(std::weak_ptr<mainWidget> widget):
    m_openglWidget(widget),
    m_initialized(false),
    m_render(true),
    m_viewWidth(720),
    m_viewHeight(576)
  {

  }
  std::weak_ptr<mainWidget> m_openglWidget;
  bool      m_initialized;
  bool      m_render;
  int         m_viewWidth;
  int         m_viewHeight;
  QMutex m_mutex;
};

enThread::enThread(std::weak_ptr<mainWidget> widget):
  m_data(std::make_shared<Data>(widget))
{

}

enThread::~enThread()
{

}

// get methods
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

// set methods
// ---------------------------------------------------------------------------
void
enThread::start()
{
  if (isRunning()) return;
  m_data->m_render = true;
  QThread::start();
}
// ---------------------------------------------------------------------------
void
enThread::stop()
{
  m_data->m_mutex.lock();
  m_data->m_render = false;
  m_data->m_mutex.unlock();
  
  quit();
  wait();
}
// ---------------------------------------------------------------------------
void
enThread::setViewPortSize(int w, int h)
{
  m_data->m_mutex.lock();
  m_data->m_viewWidth = w;
  m_data->m_viewHeight = h;
  m_data->m_mutex.unlock();
}
// ---------------------------------------------------------------------------
void
enThread::run()
{
  for (;;) {
    int w = 0, h = 0;
    bool render = true;
    m_data->m_mutex.lock();
    render = m_data->m_render;
    w = m_data->m_viewWidth;
    h = m_data->m_viewHeight;
    m_data->m_mutex.unlock();
    if (!render) break;
    std::shared_ptr<mainWidget> widget = m_data->m_openglWidget.lock();
    if (!widget) {
      break;
    }
    widget->makeCurrent();
    glViewport(0, 0, w, h);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    widget->swapBuffers();
    widget->doneCurrent();
  }
}
// ---------------------------------------------------------------------------
