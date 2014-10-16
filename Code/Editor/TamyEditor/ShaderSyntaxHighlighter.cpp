#include "ShaderSyntaxHighlighter.h"
#include "TextSyntaxHighlighter.h"


///////////////////////////////////////////////////////////////////////////////

ShaderSyntaxHighlighter::ShaderSyntaxHighlighter()
{

   m_keywordFormat.setForeground( Qt::darkBlue );
   m_keywordFormat.setFontWeight( QFont::Bold );
   QStringList keywordPatterns;
   keywordPatterns << "\\bstruct\\b" << "\\bclass\\b" << "\\bconst\\b" << "\\bbool\\b"
      << "\\bfloat\\b" << "\\bfloat2\\b" << "\\bfloat3\\b" << "\\bfloat4\\b"
      << "\\bfloat3x3\\b" << "\\bfloat3x4\\b" << "\\bfloat4x3\\b" << "\\bfloat4x4\\b" << "\\bfloat2x2\\b"
      << "\\bint\\b" << "\\bint2\\b" << "\\bint3\\b" << "\\bint4\\b"
      << "\\btexture\\b" << "\\bvoid\\b" << "\\bsampler\\b" << "\\bsampler_state\\b" 
      << "\\bhalf\\b" << "\\breturn\\b" << "\\bif\\b" << "\\belse\\b" << "\\bfor\\b"
      << "\\buniform\\b" << "\\bstatic\\b";
   foreach ( const QString &pattern, keywordPatterns ) 
   {
      TextSyntaxHighlighter::HighlightingRule& rule = appendRule();
      rule.pattern = QRegExp( pattern );
      rule.format = m_keywordFormat;
   }

   QBrush specialKeywordsBrush( QColor( 200, 64, 52 ) );
   m_specialKeywordFormat.setForeground( specialKeywordsBrush );
   m_specialKeywordFormat.setFontWeight( QFont::Bold );
   QStringList specialKeywordPatterns;
   specialKeywordPatterns << "\\b#define\\b" << "\\b#include\\b" << "\\bin\\b" << "\\bout\\b";
   foreach ( const QString &pattern, specialKeywordPatterns ) 
   {
      TextSyntaxHighlighter::HighlightingRule& rule = appendRule();
      rule.pattern = QRegExp( pattern );
      rule.format = m_specialKeywordFormat;
   }

   {
      m_classFormat.setFontWeight( QFont::Bold );
      m_classFormat.setForeground( Qt::darkMagenta );

      TextSyntaxHighlighter::HighlightingRule& rule = appendRule();
      rule.pattern = QRegExp( "\\bQ[A-Za-z]+\\b" );
      rule.format = m_classFormat;
   }

   {
      m_quotationFormat.setForeground( QColor( 50, 188, 0 ) );
      TextSyntaxHighlighter::HighlightingRule& rule = appendRule();
      rule.pattern = QRegExp( "\".*\"" );
      rule.format = m_quotationFormat;
   }

   {
      m_functionFormat.setFontItalic( true );
      m_functionFormat.setForeground( Qt::blue );
      TextSyntaxHighlighter::HighlightingRule& rule = appendRule();
      rule.pattern = QRegExp( "\\b[A-Za-z0-9_]+(?=\\()" );
      rule.format = m_functionFormat;
   }

   {
      QBrush commentsBrush( QColor( 50, 188, 0 ) );
      m_singleLineCommentFormat.setForeground( commentsBrush );
      m_multiLineCommentFormat.setForeground( commentsBrush );

      TextSyntaxHighlighter::HighlightingRule& rule = appendRule();
      rule.pattern = QRegExp( "//[^\n]*" );
      rule.format = m_singleLineCommentFormat;
   }


   m_commentStartExpression = QRegExp( "/\\*" );
   m_commentEndExpression = QRegExp( "\\*/" );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderSyntaxHighlighter::onHighlightBlock( const QString &text )
{
   // highlight multi-line comments
   setCurrentBlockState( 0 );

   int startIndex = 0;
   if ( previousBlockState() != 1 )
   {
      startIndex = m_commentStartExpression.indexIn( text );
   }

   while ( startIndex >= 0 ) 
   {
      int endIndex = m_commentEndExpression.indexIn( text, startIndex );
      int commentLength;
      if ( endIndex == -1 ) 
      {
         setCurrentBlockState( 1 );
         commentLength = text.length() - startIndex;
      } 
      else 
      {
         commentLength = endIndex - startIndex + m_commentEndExpression.matchedLength();
      }
      setFormat( startIndex, commentLength, m_multiLineCommentFormat );
      startIndex = m_commentStartExpression.indexIn( text, startIndex + commentLength );
   }
}

///////////////////////////////////////////////////////////////////////////////
