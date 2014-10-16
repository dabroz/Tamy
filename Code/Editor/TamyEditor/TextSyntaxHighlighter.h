/// @file   TamyEditor/TextSyntaxHighlighter.h
/// @brief  a custom syntax highlighter used by the TextEditWidget class
#pragma once

#include <QSyntaxHighlighter>


///////////////////////////////////////////////////////////////////////////////

/**
 * A custom syntax highlighter used by the TextEditWidget class.
 */
class TextSyntaxHighlighter : public QSyntaxHighlighter
{
   Q_OBJECT

public:
   struct HighlightingRule
   {
      QRegExp                    pattern;
      QTextCharFormat            format;
   };

private:
   QVector< HighlightingRule >   m_highlightingRules;

   HighlightingRule              m_textInstanceHighlightRule;
   bool                          m_textInstanceHighlightRuleActive;
   bool                          m_textInstanceHighlightEnabled;

public:
   /**
    * Constructor.
    */
   TextSyntaxHighlighter();
   virtual ~TextSyntaxHighlighter();


   /**
    * Highlights all instances of the specified text.
    *
    * @param text
    */
   void highlightAllInstances( const QString& text );

   /**
    * Appends a new highlighting rule.
    *
    * @return  a reference to the new rule
    */
   HighlightingRule& appendRule();

public slots:
   void toggleSelectionHighlight();

protected:
   void highlightBlock( const QString &text );

   /**
    * Implement this method if you want to provide some specific way in which certain blocks of text
    * should be highlighted.
    *
    * @param text
    */
   virtual void onHighlightBlock( const QString &text ) {}

private:
   void initializeDefaultRules();

   /**
    * Runs the specified rule.
    *
    * @param rule
    */
   void executeRule( const HighlightingRule& rule, const QString& text );
};

///////////////////////////////////////////////////////////////////////////////

