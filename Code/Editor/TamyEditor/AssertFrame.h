/// @file   TamyEditor/AssertFrame.h
/// @brief  a frame with an info about an assertion that took place on a worker thread

#include <QtWidgets\QFrame>
#include "ui_assertframe.h"
#include "core\Assert.h"
#include "core\ThreadSafe.h"
#include "EventAssert.h"


///////////////////////////////////////////////////////////////////////////////

class QCloseEvent;
class QTabWidget;

///////////////////////////////////////////////////////////////////////////////

class AssertFrame : public QFrame
{
   Q_OBJECT

private:
   Ui::AssertFrame                        m_ui;
   QTabWidget*                            m_parent;
   TThreadSafe< EventAssert::Choice >&    m_choiceFlag;
   bool                                   m_answered;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param whereabouts      description from where an assert was thrown
    * @param msg              assertion message
    * @param choiceFlag       user's decision how the assertion should be handled
    */
   AssertFrame( QTabWidget* parent, const char* whereabouts, const char* msg, TThreadSafe< EventAssert::Choice >& choiceFlag );
   ~AssertFrame();


public slots:
   void onBreak();
   void onIgnore();
   void onIgnoreAll();
};

///////////////////////////////////////////////////////////////////////////////
