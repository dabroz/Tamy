/// @file   TamyEditor/ShaderSyntaxHighlighter.h
/// @brief  material syntax highlighter

#pragma once

#include "TextSyntaxHighlighter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Material syntax highlighter.
 */
class ShaderSyntaxHighlighter : public TextSyntaxHighlighter
{
private:
   QRegExp                       m_commentStartExpression;
   QRegExp                       m_commentEndExpression;

   QTextCharFormat               m_keywordFormat;
   QTextCharFormat               m_specialKeywordFormat;
   QTextCharFormat               m_classFormat;
   QTextCharFormat               m_singleLineCommentFormat;
   QTextCharFormat               m_multiLineCommentFormat;
   QTextCharFormat               m_quotationFormat;
   QTextCharFormat               m_functionFormat;


public:
   /**
    * Constructor.
    */
   ShaderSyntaxHighlighter();

   // -------------------------------------------------------------------------
   // TextSyntaxHighlighter implementation
   // -------------------------------------------------------------------------
   void onHighlightBlock( const QString &text );
};

///////////////////////////////////////////////////////////////////////////////
