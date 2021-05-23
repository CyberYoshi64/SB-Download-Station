#include "common.hpp"
#include "init.hpp"

namespace Dialog {
	void Clear(void);
	void WaitForBool(bool* pnt, bool flg);
	void MainWGiven(std::string maintxt);
	void MainWithID(u32 id);
	void UseAButtonWGiven(std::string btnstr);
	void UseBButtonWGiven(std::string btnstr);
	void UseAButtonWithID(u32 id);
	void UseBButtonWithID(u32 id);
	void PrepareDisplay(void);
	bool IsDialogShown(void);
	u8 Show(void);
	void Display(void);
}
