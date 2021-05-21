#ifndef INIT_HPP
#define INIT_HPP

#include <3ds.h>

namespace Init {
	// Init, Mainloop & Exit.
	Result Initialize();
	Result MainLoop();
	Result Exit();
	
	void SelSound();
	void BackSound();
	void WrongSound();
	void LaunchSound();
	void StopSound();
	void SwitchSound();
	void ErrorSound();
	void WarningSound();
	void MenuSound();
	void WaitSound();
	void StopWaitSound();
}

#endif
