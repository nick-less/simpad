// generated by Fast Light User Interface Designer (fluid) version 1.0100

#ifndef controlPanel_h
#define controlPanel_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Clock.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>

class ControlPanel {
public:
  ControlPanel();
  Fl_Window *controlWin;
  Fl_Tabs *controlTab;
private:
  inline void cb_controlTab_i(Fl_Tabs*, void*);
  static void cb_controlTab(Fl_Tabs*, void*);
public:
  Fl_Group *avGroup;
  Fl_Box *soundBoarder;
  Fl_Box *videoBoarder;
  Fl_Check_Button *volCheckButton;
private:
  inline void cb_volCheckButton_i(Fl_Check_Button*, void*);
  static void cb_volCheckButton(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *micCheckButton;
private:
  inline void cb_micCheckButton_i(Fl_Check_Button*, void*);
  static void cb_micCheckButton(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *lightCheckButton;
private:
  inline void cb_lightCheckButton_i(Fl_Check_Button*, void*);
  static void cb_lightCheckButton(Fl_Check_Button*, void*);
public:
  Fl_Button *calibButton;
private:
  inline void cb_calibButton_i(Fl_Button*, void*);
  static void cb_calibButton(Fl_Button*, void*);
public:
  Fl_Slider *volSlider;
private:
  inline void cb_volSlider_i(Fl_Slider*, void*);
  static void cb_volSlider(Fl_Slider*, void*);
public:
  Fl_Slider *micSlider;
private:
  inline void cb_micSlider_i(Fl_Slider*, void*);
  static void cb_micSlider(Fl_Slider*, void*);
public:
  Fl_Slider *lightSlider;
private:
  inline void cb_lightSlider_i(Fl_Slider*, void*);
  static void cb_lightSlider(Fl_Slider*, void*);
public:
  Fl_Group *timeGroup;
  Fl_Box *dateBoarder;
  Fl_Box *timeBoarder;
  Fl_Choice *monthChoice;
private:
  inline void cb_monthChoice_i(Fl_Choice*, void*);
  static void cb_monthChoice(Fl_Choice*, void*);
  static Fl_Menu_Item menu_monthChoice[];
public:
  Fl_Counter *yearCounter;
private:
  inline void cb_yearCounter_i(Fl_Counter*, void*);
  static void cb_yearCounter(Fl_Counter*, void*);
public:
  Fl_Box *dateLabel;
  Fl_Box *timeLabel;
  Fl_Clock *sysClock;
  Fl_Value_Input *hourInput;
private:
  inline void cb_hourInput_i(Fl_Value_Input*, void*);
  static void cb_hourInput(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *minInput;
private:
  inline void cb_minInput_i(Fl_Value_Input*, void*);
  static void cb_minInput(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *secInput;
private:
  inline void cb_secInput_i(Fl_Value_Input*, void*);
  static void cb_secInput(Fl_Value_Input*, void*);
public:
  Fl_Button *plusButton;
private:
  inline void cb_plusButton_i(Fl_Button*, void*);
  static void cb_plusButton(Fl_Button*, void*);
public:
  Fl_Button *minusButton;
private:
  inline void cb_minusButton_i(Fl_Button*, void*);
  static void cb_minusButton(Fl_Button*, void*);
public:
  Fl_Box *calendarBox;
  Fl_Group *powerGroup;
  Fl_Progress *batteryState;
  Fl_Counter *criticalTrigger;
private:
  inline void cb_criticalTrigger_i(Fl_Counter*, void*);
  static void cb_criticalTrigger(Fl_Counter*, void*);
public:
  Fl_Check_Button *runCommandCheck;
private:
  inline void cb_runCommandCheck_i(Fl_Check_Button*, void*);
  static void cb_runCommandCheck(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *playSoundCheck;
private:
  inline void cb_playSoundCheck_i(Fl_Check_Button*, void*);
  static void cb_playSoundCheck(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *systemBeepCheck;
private:
  inline void cb_systemBeepCheck_i(Fl_Check_Button*, void*);
  static void cb_systemBeepCheck(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *notifyCheck;
private:
  inline void cb_notifyCheck_i(Fl_Check_Button*, void*);
  static void cb_notifyCheck(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *suspendCheck;
private:
  inline void cb_suspendCheck_i(Fl_Check_Button*, void*);
  static void cb_suspendCheck(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *backlightCheck;
private:
  inline void cb_backlightCheck_i(Fl_Check_Button*, void*);
  static void cb_backlightCheck(Fl_Check_Button*, void*);
public:
  Fl_Input *commandInput;
  Fl_Input *soundFileInput;
  Fl_Button *commandButton;
private:
  inline void cb_commandButton_i(Fl_Button*, void*);
  static void cb_commandButton(Fl_Button*, void*);
public:
  Fl_Button *soundFileButton;
private:
  inline void cb_soundFileButton_i(Fl_Button*, void*);
  static void cb_soundFileButton(Fl_Button*, void*);
public:
  Fl_Group *networkGroup;
  Fl_Box *netDeviceLabel;
  Fl_Choice *netDeviceChoice;
private:
  inline void cb_netDeviceChoice_i(Fl_Choice*, void*);
  static void cb_netDeviceChoice(Fl_Choice*, void*);
public:
  Fl_Input *hostNameInput;
private:
  inline void cb_hostNameInput_i(Fl_Input*, void*);
  static void cb_hostNameInput(Fl_Input*, void*);
public:
  Fl_Box *hwAddrLabel;
  Fl_Box *hwAddrBox;
  Fl_Box *inetAddrLabel;
  Fl_Box *netMaskLabel;
  Fl_Box *bcastAddrLabel;
  Fl_Value_Input *inetAddr0;
private:
  inline void cb_inetAddr0_i(Fl_Value_Input*, void*);
  static void cb_inetAddr0(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *inetAddr1;
private:
  inline void cb_inetAddr1_i(Fl_Value_Input*, void*);
  static void cb_inetAddr1(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *inetAddr2;
private:
  inline void cb_inetAddr2_i(Fl_Value_Input*, void*);
  static void cb_inetAddr2(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *inetAddr3;
private:
  inline void cb_inetAddr3_i(Fl_Value_Input*, void*);
  static void cb_inetAddr3(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *netMask0;
private:
  inline void cb_netMask0_i(Fl_Value_Input*, void*);
  static void cb_netMask0(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *netMask1;
private:
  inline void cb_netMask1_i(Fl_Value_Input*, void*);
  static void cb_netMask1(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *netMask2;
private:
  inline void cb_netMask2_i(Fl_Value_Input*, void*);
  static void cb_netMask2(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *netMask3;
private:
  inline void cb_netMask3_i(Fl_Value_Input*, void*);
  static void cb_netMask3(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *bcastAddr0;
private:
  inline void cb_bcastAddr0_i(Fl_Value_Input*, void*);
  static void cb_bcastAddr0(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *bcastAddr1;
private:
  inline void cb_bcastAddr1_i(Fl_Value_Input*, void*);
  static void cb_bcastAddr1(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *bcastAddr2;
private:
  inline void cb_bcastAddr2_i(Fl_Value_Input*, void*);
  static void cb_bcastAddr2(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *bcastAddr3;
private:
  inline void cb_bcastAddr3_i(Fl_Value_Input*, void*);
  static void cb_bcastAddr3(Fl_Value_Input*, void*);
public:
  Fl_Button *devInfoButton;
private:
  inline void cb_devInfoButton_i(Fl_Button*, void*);
  static void cb_devInfoButton(Fl_Button*, void*);
public:
  Fl_Group *wirelessGroup;
  Fl_Choice *wifiDeviceChoice;
private:
  inline void cb_wifiDeviceChoice_i(Fl_Choice*, void*);
  static void cb_wifiDeviceChoice(Fl_Choice*, void*);
public:
  Fl_Choice *wifiChannelChoice;
private:
  inline void cb_wifiChannelChoice_i(Fl_Choice*, void*);
  static void cb_wifiChannelChoice(Fl_Choice*, void*);
  static Fl_Menu_Item menu_wifiChannelChoice[];
public:
  Fl_Choice *wifiModeChoice;
private:
  inline void cb_wifiModeChoice_i(Fl_Choice*, void*);
  static void cb_wifiModeChoice(Fl_Choice*, void*);
  static Fl_Menu_Item menu_wifiModeChoice[];
public:
  Fl_Input *essidInput;
private:
  inline void cb_essidInput_i(Fl_Input*, void*);
  static void cb_essidInput(Fl_Input*, void*);
public:
  Fl_Input *freqInput;
private:
  inline void cb_freqInput_i(Fl_Input*, void*);
  static void cb_freqInput(Fl_Input*, void*);
public:
  Fl_Input *rateInput;
private:
  inline void cb_rateInput_i(Fl_Input*, void*);
  static void cb_rateInput(Fl_Input*, void*);
public:
  Fl_Box *wifiDeviceLabel;
  Fl_Box *wifiChannelLabel;
  Fl_Box *wifiModeLabel;
  Fl_Button *wifiInfoButton;
private:
  inline void cb_wifiInfoButton_i(Fl_Button*, void*);
  static void cb_wifiInfoButton(Fl_Button*, void*);
public:
  Fl_Group *cs3Group;
  Fl_Box *rs232Boarder;
  Fl_Choice *baudRateChoice;
private:
  inline void cb_baudRateChoice_i(Fl_Choice*, void*);
  static void cb_baudRateChoice(Fl_Choice*, void*);
  static Fl_Menu_Item menu_baudRateChoice[];
public:
  Fl_Box *irdaBoarder;
  Fl_Box *miscBoarder;
  Fl_Check_Button *rs232CheckBox;
private:
  inline void cb_rs232CheckBox_i(Fl_Check_Button*, void*);
  static void cb_rs232CheckBox(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *enIrdaCheckBox;
private:
  inline void cb_enIrdaCheckBox_i(Fl_Check_Button*, void*);
  static void cb_enIrdaCheckBox(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *irdaModeCheckBox;
private:
  inline void cb_irdaModeCheckBox_i(Fl_Check_Button*, void*);
  static void cb_irdaModeCheckBox(Fl_Check_Button*, void*);
public:
  Fl_Check_Button *greeLedCheckBox;
  Fl_Check_Button *dectCheckBox;
private:
  inline void cb_dectCheckBox_i(Fl_Check_Button*, void*);
  static void cb_dectCheckBox(Fl_Check_Button*, void*);
public:
  Fl_Group *infoGroup;
  Fl_Box *bootloaderBox;
  Fl_Box *cpuInfoBox;
  Fl_Box *kernelBox;
  Fl_Box *glibcBox;
  Fl_Box *tuxIcon;
  Fl_Button *okayButton;
private:
  inline void cb_okayButton_i(Fl_Button*, void*);
  static void cb_okayButton(Fl_Button*, void*);
public:
  Fl_Button *applyButton;
private:
  inline void cb_applyButton_i(Fl_Button*, void*);
  static void cb_applyButton(Fl_Button*, void*);
public:
  Fl_Button *cancelButton;
private:
  inline void cb_cancelButton_i(Fl_Button*, void*);
  static void cb_cancelButton(Fl_Button*, void*);
public:
  Fl_Window *infoDlgWin;
  Fl_Output *infoDevOutput;
  Fl_Button *okayInfoButton;
private:
  inline void cb_okayInfoButton_i(Fl_Button*, void*);
  static void cb_okayInfoButton(Fl_Button*, void*);
public:
  virtual void doOkay();
  virtual void doApply();
  virtual void doCancel();
  virtual void changeTab();
  virtual void changeVol();
  virtual void changeMic();
  virtual void changeLight();
  virtual void volMute();
  virtual void micMute();
  virtual void lightMute();
  virtual void doCalibration();
  virtual void show( int argc, char** argv);
  virtual void changeMonth();
  virtual void changeYear();
  virtual void plusTime();
  virtual void minusTime();
  virtual void openCommand();
  virtual void openSoundFile();
  virtual void enableCommand();
  virtual void enableSoundFile();
  virtual void changeNetDevice();
  virtual void showDevInfoDlg();
  virtual void changeWifiDevice();
  virtual void showWifiInfoDlg();
  virtual void changeApplyButton();
  virtual void doInfoOkay();
};
#endif