/*
  This file is part of the Grantlee template system.

  Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 3 only, as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License version 3 for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NODE_H
#define NODE_H

#include "context.h"

#include <QStringList>
#include "filterexpression.h"
#include "safestring.h"

#include "grantlee.h"

#include "grantlee_export.h"

namespace Grantlee
{

class NodeList;

class NodePrivate;

/**
Abstract base class for nodes.
*/
class GRANTLEE_EXPORT Node : public QObject
{
  Q_OBJECT
public:
  Node( QObject *parent = 0 );
  virtual ~Node();

  // This can't be const because CycleNode needs to change on each render.
  virtual QString render( Context *c ) = 0;

  virtual bool isPersistent() {
    return isRepeatable();
  }
  virtual bool isRepeatable() {
    return false;
  }

  virtual bool mustBeFirst() {
    return false;
  }

protected:
  QString renderValueInContext( const QVariant &input, Grantlee::Context *c );

private:
  Q_DECLARE_PRIVATE( Node )
  NodePrivate *d_ptr;
};

class GRANTLEE_EXPORT NodeList : public QList<Grantlee::Node*>
{
public:
  NodeList();
  NodeList( const NodeList &list );
  NodeList( const QList<Grantlee::Node *> &list );
  ~NodeList();

  void append( Grantlee::Node* node );
  void append( QList<Grantlee::Node*> nodeList );
  bool containsNonText() const;

  template <typename T> QList<T> findChildren() {
    QList<T> children;
    QList<Grantlee::Node*>::const_iterator it;
    const QList<Grantlee::Node*>::const_iterator first = constBegin();
    const QList<Grantlee::Node*>::const_iterator last = constEnd();
    for ( it = first; it != last; ++it ) {
      T *object = qobject_cast<T>( *it );
      if ( object ) {
        children << object;
      }
      children << object->findChildren<T>();
    }
    return children;
  }

  QString render( Context *c );

protected:
  QString mutableRender( Context *c );

private:
  bool m_containsNonText;
};

class AbstractNodeFactoryPrivate;

/**
Base class for tags. Downstreams can use this to create custom tags.
This can also be inherited by qtscript objects to define tags in script.

Each tag is essentially a factory class for the type of node it creates.

Maybe this should be AbstractNodeFactory ... IfNodeFactory.

Users implement AbstractNodeFactory to create new Nodes. and register their factory with the library?

Alternatively I could use a prototype pattern?
*/
class GRANTLEE_EXPORT AbstractNodeFactory : public QObject
{
  Q_OBJECT
public:
  AbstractNodeFactory( QObject* parent = 0 );
  virtual ~AbstractNodeFactory();

  /**
  Advances the parser as needed for block style tags.
  */
  virtual Node* getNode( const QString &tagContent, Parser *p ) const = 0;

protected:
  Q_INVOKABLE QStringList smartSplit( const QString &str ) const;

protected:
  QList<Variable> getVariableList( const QStringList &list ) const;
  QList<FilterExpression> getFilterExpressionList( const QStringList &list, Parser *p ) const;

private:
  Q_DECLARE_PRIVATE( AbstractNodeFactory )
  AbstractNodeFactoryPrivate *d_ptr;
};

class GRANTLEE_EXPORT TextNode : public Node
{
  Q_OBJECT
public:
  TextNode( const QString &content, QObject *parent = 0 );

  QString render( Context *c ) {
    Q_UNUSED( c );
    return m_content;
  }

  void appendContent( const QString &content ) {
    m_content += content;
  }

private:
  QString m_content;
};

class GRANTLEE_EXPORT VariableNode : public Node
{
  Q_OBJECT
public:
  VariableNode( const FilterExpression &fe, QObject *parent = 0 );

  QString render( Context *c );

private:
  FilterExpression m_filterExpression;

};

// TODO: figure out if I can use the same QMetaType tricks described in qt:qtscript for QPoint.
// Define a macro to wrap non-qobject cpp classes.

}

#endif

