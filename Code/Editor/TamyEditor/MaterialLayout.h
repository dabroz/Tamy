/// @file   TamyEditor/MaterialLayout.h
/// @brief  material pipeline layout
#pragma once

#include "GraphLayout.h"
#include "core-Renderer\MaterialProfile.h"
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class MaterialNode;
class GraphBlock;
class MNInputStruct;
class MNOutputStruct;

///////////////////////////////////////////////////////////////////////////////

/**
 * Layout for a rendering pipeline
 */
class MaterialLayout : public TGraphLayout< MaterialNode >
{
private:
   int&           m_activeRoutineIdx;

public:
   /**
    * Constructor.
    *
    * @param graphBuilder
    * @param activeRoutineIdx    a reference to a MaterialEditor::m_activeRoutineIdx field
    */
   MaterialLayout( GraphBuilder< MaterialNode >& graphBuilder, int& activeRoutineIdx );

   /**
   * Defines the mandatory sockets the specified routine's input structure should contain.
   *
   * @param inputStruct
   * @param routineIdx
   */
   static void createMandatorySockets( MNInputStruct* inputStruct, int routineIdx );

   /**
   * Defines the mandatory sockets the specified routine's output structure should contain.
   *
   * @param outputStruct
   * @param routineIdx
   */
   static void createMandatorySockets( MNOutputStruct* outputStruct, int routineIdx );

   /**
    * Returns the index of the routine currently active in the parent material editor.
    */
   inline int getActiveRoutineIdx() const {
      return m_activeRoutineIdx;
   }

protected:
   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void initSocketsFactory( SocketsFactory& factory );
};

///////////////////////////////////////////////////////////////////////////////

class GNAAddVertexShaderInputSockets
{
public:
   static void onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block );
};

///////////////////////////////////////////////////////////////////////////////

class GNAAddVertexShaderOutputSockets
{
public:
   static void onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block );
};

///////////////////////////////////////////////////////////////////////////////

class GNAAddFragmentShaderInputSockets
{
public:
   static void onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block );
};

///////////////////////////////////////////////////////////////////////////////

class GNAAddFragmentShaderOutputSockets
{
public:
   static void onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block );
};

///////////////////////////////////////////////////////////////////////////////

class AddVec2Input : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*         m_node;
   std::string             m_name;

public:
   AddVec2Input( MNOutputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddVec3Input : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*         m_node;
   std::string             m_name;

public:
   AddVec3Input( MNOutputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////


class AddVec4Input : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*         m_node;
   std::string             m_name;

public:
   AddVec4Input( MNOutputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddColorInput : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*         m_node;
   std::string             m_name;

public:
   AddColorInput( MNOutputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddVec2Output : public QAction
{
   Q_OBJECT

private:
   MNInputStruct*          m_node;
   std::string             m_name;

public:
   AddVec2Output( MNInputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddVec3Output : public QAction
{
   Q_OBJECT

private:
   MNInputStruct*          m_node;
   std::string             m_name;

public:
   AddVec3Output( MNInputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddVec4Output : public QAction
{
   Q_OBJECT

private:
   MNInputStruct*          m_node;
   std::string             m_name;

public:
   AddVec4Output( MNInputStruct* node, const char* name );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class ResetInputs : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*         m_node;
   GraphLayout*            m_layout;

public:
   ResetInputs( MNOutputStruct* node, GraphLayout* layout );

   public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class ResetOutputs : public QAction
{
   Q_OBJECT

private:
   MNInputStruct*          m_node;
   GraphLayout*            m_layout;

public:
   ResetOutputs( MNInputStruct* node, GraphLayout* layout );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddTriangleMeshLayout : public QAction
{
   Q_OBJECT

private:
   MNInputStruct*       m_node;

public:
   AddTriangleMeshLayout( MNInputStruct* node );

   public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddSkinningLayout : public QAction
{
   Q_OBJECT

private:
   MNInputStruct*       m_node;

public:
   AddSkinningLayout( MNInputStruct* node );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddColorLayout : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*       m_node;

public:
   AddColorLayout( MNOutputStruct* node );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddGBufferLayout : public QAction
{
   Q_OBJECT

private:
   MNOutputStruct*       m_node;

public:
   AddGBufferLayout( MNOutputStruct* node );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

