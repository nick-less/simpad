#include"controlPanelImpl.hpp"


int main(int argc, char **argv)
{
  ControlPanelImpl *ctrlPanel = new ControlPanelImpl();
  ctrlPanel->show( argc, argv );

  return Fl::run();
}
