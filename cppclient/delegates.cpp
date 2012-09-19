#include "delegates.h"

ComboDelegate::ComboDelegate(QObject *parent)
  : QItemDelegate(parent)
{ 
  return;
}

QWidget *ComboDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
  
