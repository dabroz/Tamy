#include "core-MVC\ModelView.h"
#include "core-MVC\Model.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

ModelView::ModelView()
{
}

///////////////////////////////////////////////////////////////////////////////

ModelView::~ModelView()
{
   while( m_models.empty() == false )
   {
      Model* model = m_models.front();
      m_models.popFront();
      model->detachListener( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ModelView::onAttachedToModel( Model& model )
{
   List< Model* >::iterator it = ListUtils::find( m_models, &model );
   if ( it.isEnd() )
   {
      m_models.pushBack( &model );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ModelView::onDetachedFromModel( Model& model )
{
   List< Model* >::iterator it = ListUtils::find( m_models, &model );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
      resetContents( model );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool ModelView::isAttachedToModel() const
{
   return !m_models.empty();
}

///////////////////////////////////////////////////////////////////////////////
