#include "HexEditor.h"


///////////////////////////////////////////////////////////////////////////////

HexEditor::HexEditor( QWidget* parentWidget, ulong initialValue )
   : QLineEdit( parentWidget )
{
   QString tmpStr;
   tmpStr.sprintf( "%08X", initialValue );
   setText( tmpStr );

   connect( this, SIGNAL( editingFinished() ), this, SLOT( valChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////

void HexEditor::valChanged()
{
   // read the entered text and parse it as a number, and then - output it as the number
   ulong newVal = getValue();

   QString tmpStr;
   tmpStr.sprintf( "%08X", newVal );
   setText( tmpStr );
}

///////////////////////////////////////////////////////////////////////////////

ulong HexEditor::getValue() const
{
   ulong val = 0;
   sscanf( text().toStdString().c_str(), "%08X", &val );
   return val;
}

///////////////////////////////////////////////////////////////////////////////
