#include "BinaryDock.h"

namespace analyzer {
  namespace gui {
    BinaryDock::BinaryDock(QWidget *parent, Qt::WindowFlags flags)
      :QDockWidget(tr("Binary View"), parent, flags)
    {
      this->setup();
    }

    BinaryDock::~BinaryDock()
    {
    }

    void BinaryDock::setup()
    {
    }
  }
}