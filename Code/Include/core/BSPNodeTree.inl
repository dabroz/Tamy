#ifndef _BSP_NODE_TREE_H
#error "This file can only be included from BSPNodeTree.h"
#else


///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
BSPNodeTree<TPolygon>::BSPNodeTree( const Array<TPolygon*>& geometry )
{
   if ( geometry.size() == 0 ) 
   {
      return;
   }

   m_root = new Node();
   createSubtree(m_root, geometry);
}

///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
BSPNodeTree<TPolygon>::~BSPNodeTree( )
{
   delete m_root; m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
void BSPNodeTree<TPolygon>::query( const Vector& pos, Array<TPolygon*>& results, Node* checkedNode )
{
   if ( checkedNode == NULL ) 
   {
      checkedNode = m_root;
   }

   const FastFloat planeDist = checkedNode->splitPlane.dotCoord( pos );

   // determine the order in which we're gonna traverse the subtree
   Node* nodeToCheckFirst;
   Node* nodeToCheckSecond;
   if ( planeDist < Float_0 ) // behind the plane
   {
      nodeToCheckFirst = checkedNode->front;
      nodeToCheckSecond = checkedNode->back;
   }
   else // in front of or on the plane
   {
      nodeToCheckFirst = checkedNode->back;
      nodeToCheckSecond = checkedNode->front;
   }

   // query the subtree
   if ( nodeToCheckFirst != NULL )
   {
      query( pos, results, nodeToCheckFirst );
   }

   results.copyFrom( checkedNode->geometry );

   if ( nodeToCheckSecond != NULL )
   {
      query( pos, results, nodeToCheckSecond );
   }
}

///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
void BSPNodeTree<TPolygon>::createSubtree( Node* currNode, const Array<TPolygon*>& geometry )
{
   unsigned int selectedSplitPolyIdx = 0;
   TPolygon* splittingPolygon = geometry[selectedSplitPolyIdx];

   currNode->splitPlane.setFromPoints( splittingPolygon->vertex(0), splittingPolygon->vertex(2), splittingPolygon->vertex(1) );

   currNode->geometry.push_back(splittingPolygon);

   Array<TPolygon*> backPolys;
   Array<TPolygon*> frontPolys;
   unsigned int polysCount = geometry.size();
   for (unsigned int i = 0; i < polysCount; ++i)
   {
      if (selectedSplitPolyIdx == i) 
      {
         continue;
      }
      classifyPolygonAgainstPlane( currNode->splitPlane, geometry[i], currNode, backPolys, frontPolys );
   }

   if (backPolys.size() > 0)
   {
      currNode->back = new Node();
      createSubtree(currNode->back, backPolys);
   }

   if (frontPolys.size() > 0)
   {
      currNode->front = new Node();
      createSubtree(currNode->front, frontPolys);
   }
}

///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
void BSPNodeTree<TPolygon>::classifyPolygonAgainstPlane( const Plane& splittingPlane, TPolygon* classifiedPoly, Node* currNode, Array<TPolygon*>& outBackPolys, Array<TPolygon*>& outFrontPolys )
{
   PlaneClassification classification = classifiedPoly->classifyAgainsPlane(splittingPlane);
   switch (classification)
   {
   case PPC_BACK:
      {
         outBackPolys.push_back(classifiedPoly);
         break;
      }

   case PPC_FRONT:
      {
         outFrontPolys.push_back(classifiedPoly);
         break;
      }

   case PPC_COPLANAR:
      {
         currNode->geometry.push_back(classifiedPoly);
         break;
      }

   case PPC_SPANNING:
      {
         Array<TPolygon*> frontSplit;
         Array<TPolygon*> backSplit;
         classifiedPoly->split(splittingPlane, frontSplit, backSplit);
         delete classifiedPoly;

         if (frontSplit.size() > 0) {outFrontPolys.copyFrom(frontSplit);}
         if (backSplit.size() > 0) {outBackPolys.copyFrom(backSplit);}
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
BSPNodeTree<TPolygon>::Node::Node( )
   : back(NULL)
   , front(NULL) 
{}

///////////////////////////////////////////////////////////////////////////////

template <class TPolygon>
BSPNodeTree<TPolygon>::Node::~Node( )
{
   delete back; back = NULL;
   delete front; front = NULL;

   unsigned int count = geometry.size();
   for (unsigned int i = 0; i < count; ++i)
   {
      delete geometry[i];
   }
   geometry.clear();                       
}

///////////////////////////////////////////////////////////////////////////////

#endif // _BSP_NODE_TREE_H
