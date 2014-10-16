#pragma once


#include <sstream>
#include <list>
#include <deque>


///////////////////////////////////////////////////////////////////////////////

/**
 * This class will traverse the hierarchy and write out its contents.
 * All that it requires is that each hierarchy node have the following
 * public method:
 * const std::list<NodeType*>& getChildren() const;
 *
 * The method should return all the children of a given node.
 */
template<class NodeType>
class HierarchyWriter
{
public:
   virtual ~HierarchyWriter() {}

   /**
    * Invoke this method to get a description of the hierarchy
    */
   std::string operator()(NodeType& entity)
   {
      std::list<NodeType*> hierarchy = linearizeHierarchy(entity);

      std::stringstream result;

      for (std::list<NodeType*>::iterator it = hierarchy.begin(); 
           it != hierarchy.end(); ++it)
      {
         writeSingle(result, **it);
         result << std::endl;
      }

      return result.str();
   }

protected:
   /** 
    * The implementation of this class should implement this method
    * so that it puts the description of the node passed as the second parameter
    * to the outpu stream passed in the first parameter.
    */
   virtual void writeSingle(std::ostream& stream, NodeType& node) = 0;

private:
   std::list<NodeType*> linearizeHierarchy(NodeType& entity)
   {
      std::list<NodeType*> result;
      std::deque<NodeType*> entitiesQueue;
      entitiesQueue.push_back(&entity);

      while(entitiesQueue.size() > 0)
      {
         NodeType* currEntity = entitiesQueue.back();
         entitiesQueue.pop_back();

         result.push_back(currEntity);

         for (std::list<NodeType*>::const_iterator it = currEntity->getChildren().begin();
              it != currEntity->getChildren().end(); ++it)
         {
            entitiesQueue.push_back(*it);
         }
      }

      return result;
   }
};

///////////////////////////////////////////////////////////////////////////////
