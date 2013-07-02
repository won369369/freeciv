/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifndef FC__CITYREP_H
#define FC__CITYREP_H

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

extern "C" {
#include "cityrep_g.h"
}

//common
#include "city.h"

//client
#include "citydlg_common.h"
#include "cityrepdata.h"
#include "climisc.h"
#include "global_worklist.h"

//agents
#include "cma_fec.h"

// gui-qt
#include "sprite.h"

// Qt
#include <QAbstractListModel>
#include <QItemDelegate>
#include <QTreeView>
#include <QWidget>
#include <QMenu>

#define NEG_VAL(x)  ((x)<0 ? (x) : (-x))
#define CMA_NONE        (10000)
#define CMA_CUSTOM      (10001)

class QMenu;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QSortFilterProxyModel;
class QTableWidget;
class QVBoxLayout;
class city_report;

/***************************************************************************
  Item delegate for painting in model of city table
***************************************************************************/
class city_item_delegate:public QItemDelegate {
  Q_OBJECT

public:
 city_item_delegate(QObject *parent);
 ~city_item_delegate() {}
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
 QSize sizeHint (const QStyleOptionViewItem & option,
                 const QModelIndex & index) const;
private:
  int item_height;
};

/***************************************************************************
  Single item in model of city view table
***************************************************************************/
class city_item: public QObject {
Q_OBJECT

public:
  city_item(struct city *pcity);
  inline int columnCount() const { return NUM_CREPORT_COLS; }
  QVariant data(int column, int role = Qt::DisplayRole) const;
  bool setData(int column, const QVariant &value, int role = Qt::DisplayRole);
  struct city* get_city();
private:
  struct city *i_city;
};

/***************************************************************************
  City model
***************************************************************************/
class city_model : public QAbstractListModel
{
  Q_OBJECT
public:
  city_model(QObject *parent = 0);
  ~city_model();
  inline int rowCount(const QModelIndex &index = QModelIndex()) const {
    Q_UNUSED(index);
    return city_list.size();
  }
  int columnCount(const QModelIndex &parent = QModelIndex()) const {
    Q_UNUSED(parent);
    return NUM_CREPORT_COLS;
  }
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::DisplayRole);
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const;
  QVariant menu_data(int section) const;
  QVariant hide_data(int section) const;
  void populate();
  void city_changed(struct city *pcity);
  void all_changed();
private slots:
  void notify_city_changed(int row);
private:
  QList<city_item *> city_list;
};

/***************************************************************************
  City widget to show city model
***************************************************************************/
class city_widget: public QTreeView
{
  Q_OBJECT
  city_model *list_model;
  QSortFilterProxyModel *filter_model;
  city_item_delegate *c_i_d;
  city_report *cr;
  QMenu select_menu;
  QMenu production_menu;
  enum menu_labels {
    CHANGE_PROD_NOW,
    CHANGE_PROD_NEXT,
    CHANGE_PROD_LAST,
    CHANGE_PROD_BEF_LAST,
    CMA,
    SELL,
    WORKLIST_ADD,
    WORKLIST_CHANGE,
    };
public:
  city_widget(city_report *ctr);
  ~city_widget();
  city_model *get_model() const;
  QList<city *> selected_cities;
  void update_model();
  void update_city(struct city *pcity);
public slots:
  void display_header_menu(const QPoint &);
  void hide_columns();
  void city_doubleclick(const QModelIndex& index);
  void city_view();
  void clear_worlist();
  void cities_selected(const QItemSelection &sl, const QItemSelection &ds);
  void display_list_menu(const QPoint&);
  void buy();
  void center();

private:
  void gen_cma_labels(QMap<QString, int> &list);
  void gen_worklist_labels(QMap<QString, int> &list);
  void gen_production_labels(menu_labels which, QMap<QString, cid> &list, 
                             bool append_units, bool append_wonders, 
                             TestCityFunc test_func);
  void fill_data(menu_labels which,QMap<QString, cid> &custom_labels, QMenu *menu);
  void fill_production_menus(city_widget::menu_labels what,
                  QMap<QString, cid> &custom_labels,
                  TestCityFunc test_func, QMenu *menu);

};

/***************************************************************************
  Widget to show as tab widget in cities view.
***************************************************************************/
class city_report: public QWidget
{
  Q_OBJECT
  city_widget *city_wdg;
  QVBoxLayout *layout;
public:
  city_report();
  ~city_report();
  void update_report();
  void update_city(struct city *pcity);
  void init();
private:
  int index;
};

#endif /* FC__CITYREP_H */
