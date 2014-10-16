#include "TextSyntaxHighlighter.h"


///////////////////////////////////////////////////////////////////////////////

TextSyntaxHighlighter::TextSyntaxHighlighter()
   : QSyntaxHighlighter( (QTextDocument*)NULL )
   , m_textInstanceHighlightRuleActive( false )
   , m_textInstanceHighlightEnabled( false )
{
   initializeDefaultRules();
}

///////////////////////////////////////////////////////////////////////////////

TextSyntaxHighlighter::~TextSyntaxHighlighter()
{
}

///////////////////////////////////////////////////////////////////////////////

void TextSyntaxHighlighter::initializeDefaultRules()
{
   // selected text instances highlight
   m_textInstanceHighlightRule.format.setBackground( QColor( 91, 203, 255 ) );
   m_textInstanceHighlightRule.format.setForeground( QColor( 255, 255, 255 ) );
   m_textInstanceHighlightRuleActive = false;
   m_textInstanceHighlightEnabled = false;
}

///////////////////////////////////////////////////////////////////////////////

void TextSyntaxHighlighter::highlightAllInstances( const QString& text )
{
   QString pattern = tr( "\\b" ) + text + tr( "\\b" );

   m_textInstanceHighlightRule.pattern = QRegExp( pattern );
   m_textInstanceHighlightRuleActive = text.length() > 0;

   if ( m_textInstanceHighlightEnabled )
   {
      rehighlight();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TextSyntaxHighlighter::toggleSelectionHighlight()
{
   m_textInstanceHighlightEnabled = !m_textInstanceHighlightEnabled;

   if ( !m_textInstanceHighlightEnabled )
   {
      // unhighlight all
      m_textInstanceHighlightRuleActive = false;
      rehighlight();
   }
}

///////////////////////////////////////////////////////////////////////////////

TextSyntaxHighlighter::HighlightingRule& TextSyntaxHighlighter::appendRule()
{
   m_highlightingRules.append( HighlightingRule() );
   return m_highlightingRules.back();
}

///////////////////////////////////////////////////////////////////////////////

void TextSyntaxHighlighter::highlightBlock( const QString &text )
{
   // highlight keywords
   foreach ( const HighlightingRule& rule, m_highlightingRules ) 
   {
      executeRule( rule, text );
   }

   // perform implementation-specific highlighting
   onHighlightBlock( text );

   // highlight all appearances of a selected word
   if ( m_textInstanceHighlightRuleActive )
   {
      executeRule( m_textInstanceHighlightRule, text );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TextSyntaxHighlighter::executeRule( const HighlightingRule& rule, const QString& text )
{
   QRegExp expression( rule.pattern );
   int index = expression.indexIn( text );
   while ( index >= 0 ) 
   {
      int length = expression.matchedLength();
      if ( length <= 0 )
      {
         break;
      }
      setFormat( index, length, rule.format );
      index = expression.indexIn( text, index + length );
   }
}

///////////////////////////////////////////////////////////////////////////////
