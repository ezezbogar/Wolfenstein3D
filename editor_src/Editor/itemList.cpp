#include "itemList.h"


#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

ItemList::ItemList(int pieceSize,std::vector<Item>& x , QWidget *parent)
    : QListWidget(parent), itemSize(pieceSize), items(x) {
    setDragEnabled(true);
    setViewMode(QListView::IconMode);
    setIconSize(QSize(pieceSize, pieceSize));
    setSpacing(10);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    this->loadImages();
}

void ItemList::loadImages(){

    for (auto &i : items){
        QListWidgetItem *pieceItem = new QListWidgetItem(this);
        pieceItem->setIcon(QIcon(i.pixmap));
        pieceItem->setData(Qt::UserRole, QVariant(i.pixmap));
        pieceItem->setData(Qt::UserRole+1, i.id);
        pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    }
}

void ItemList::startDrag(Qt::DropActions )
{
    QListWidgetItem *item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
    int id = item->data(Qt::UserRole+1).toInt();

    dataStream << pixmap << id;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(ItemList::editorMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);
    drag->exec(Qt::MoveAction);

    /*if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        delete takeItem(row(item));
    */
}

