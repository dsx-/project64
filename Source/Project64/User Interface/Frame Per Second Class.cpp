#include "..\\User Interface.h"
#include "../N64 System/N64 Types.h"

#include <windows.h> //for QueryPerformanceCounter

CFramePerSecond::CFramePerSecond (CNotification * Notification):
	_Notify(Notification)
{
	m_iFrameRateType = _Settings->LoadDword(UserInterface_FrameDisplayType);
	m_ScreenHertz = _Settings->LoadDword(GameRunning_ScreenHertz);
	_Settings->RegisterChangeCB(UserInterface_FrameDisplayType,this,(CSettings::SettingChangedFunc)FrameRateTypeChanged);
	_Settings->RegisterChangeCB(GameRunning_ScreenHertz,this,(CSettings::SettingChangedFunc)ScreenHertzChanged);
	
	if (m_ScreenHertz == 0)
	{
		m_ScreenHertz = 60;
	}
	
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	Frequency = Freq.QuadPart;
	Reset(true);
}

CFramePerSecond::~CFramePerSecond()
{
	_Settings->UnregisterChangeCB(UserInterface_FrameDisplayType,this,(CSettings::SettingChangedFunc)FrameRateTypeChanged);
	_Settings->UnregisterChangeCB(GameRunning_ScreenHertz,this,(CSettings::SettingChangedFunc)ScreenHertzChanged);
}

void CFramePerSecond::Reset (bool ClearDisplay) {
	CurrentFrame = 0;
	LastFrame = 0;

	for (int count = 0; count < NoOfFrames; count ++) {
		Frames[count] = 0;
	}
	if (ClearDisplay)
	{
		_Notify->DisplayMessage2("");
		return;
	}
	
	if (m_iFrameRateType == FR_VIs) 
	{
		DisplayViCounter(0);
	}
}

void CFramePerSecond::UpdateViCounter ( void )
{
	if (m_iFrameRateType != FR_VIs && m_iFrameRateType != FR_PERCENT)
	{
		return;
	}
	if ((CurrentFrame & 7) == 0) {
		LARGE_INTEGER Time;
		QueryPerformanceCounter(&Time);
		Frames[(CurrentFrame >> 3) % NoOfFrames] = Time.QuadPart - LastFrame;
		LastFrame = Time.QuadPart;	
		DisplayViCounter(0);
	}
	CurrentFrame += 1;
}

void CFramePerSecond::DisplayViCounter(DWORD FrameRate) {
	if (m_iFrameRateType == FR_VIs)
	{
		if (FrameRate != 0) {
			_Notify->DisplayMessage2("VI/s: %d.00", FrameRate);
		} else {
			if (CurrentFrame > (NoOfFrames << 3)) {
				__int64 Total;
				
				Total = 0;
				for (int count = 0; count < NoOfFrames; count ++) {
					Total += Frames[count];
				}
				_Notify->DisplayMessage2("VI/s: %.2f", Frequency/ ((double)Total / (NoOfFrames << 3)));
			} else {
				_Notify->DisplayMessage2("VI/s: -.--");
			}
		}
	}
	if (m_iFrameRateType == FR_PERCENT)
	{
		float Percent;
		if (FrameRate != 0) {
			Percent = ((float)FrameRate) / m_ScreenHertz;
		} else {
			if (CurrentFrame > (NoOfFrames << 3)) {
				__int64 Total;
				
				Total = 0;
				for (int count = 0; count < NoOfFrames; count ++) {
					Total += Frames[count];
				}
				Percent = ((float)(Frequency/ ((double)Total / (NoOfFrames << 3)))) / m_ScreenHertz;
			} else {
				_Notify->DisplayMessage2("");
				return;
			}
		}
		_Notify->DisplayMessage2("%.1f %%",Percent * 100);
	}
}

void CFramePerSecond::FrameRateTypeChanged (CFramePerSecond * _this)
{
	_this->m_iFrameRateType    = _Settings->LoadDword(UserInterface_FrameDisplayType);
	_this->Reset(true);
}

void CFramePerSecond::ScreenHertzChanged (CFramePerSecond * _this)
{
	_this->m_ScreenHertz = _Settings->LoadDword(GameRunning_ScreenHertz);
	_this->Reset(true);
}

void CFramePerSecond::UpdateDlCounter  ( void )
{
	if (m_iFrameRateType != FR_DLs)
	{
		return;
	}
	if ((CurrentFrame & 3) == 0) {
		LARGE_INTEGER Time;
		QueryPerformanceCounter(&Time);
		Frames[(CurrentFrame >> 2) % NoOfFrames] = Time.QuadPart - LastFrame;
		LastFrame = Time.QuadPart;	
		DisplayDlCounter(0);
	}
	CurrentFrame += 1;
}

void CFramePerSecond::DisplayDlCounter(DWORD FrameRate) {
	if (m_iFrameRateType != FR_DLs)
	{
		return;
	}
	if (FrameRate != 0) {
		_Notify->DisplayMessage2("DL/s: %d.00", FrameRate);
	} else {
		if (CurrentFrame > (NoOfFrames << 2)) {
			__int64 Total;
			
			Total = 0;
			for (int count = 0; count < NoOfFrames; count ++) {
				Total += Frames[count];
			}
			_Notify->DisplayMessage2("DL/s: %.1f", Frequency/ ((double)Total / (NoOfFrames << 2)));
		} else {
			_Notify->DisplayMessage2("DL/s: -.--");
		}
	}
}
