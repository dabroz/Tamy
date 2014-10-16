#pragma once

#include "core-AppFlow\ApplicationManager.h"
#include "core\Point.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class ApplicationManagerMock : public ApplicationManager
{
protected:
   ProcessingCode onStep() {return APC_APPLICATION;}

   float getTimeElapsed() {return 1;}
};

///////////////////////////////////////////////////////////////////////////////
