#ifndef MAP_H
#define MAP_H

#include "mapelement.h"
#include <QWidget>
#include  <QVector>


class Map : public QWidget
{
    Q_OBJECT
public:
    explicit Map(unsigned x = 5, unsigned y= 5, QWidget *parent = nullptr);

signals:

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:

    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position) const;
    QVector<MapElement> elements;
    QRect focused;
};

#endif // MAP_H
