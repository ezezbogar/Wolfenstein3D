#ifndef TRASHBIN_H
#define TRASHBIN_H

#include <QWidget>
#include <QVector>
#include <QPaintEvent>
#include <atomic>

// Modela un tacho de basura, recibe drops.
class TrashBin : public QWidget {
    Q_OBJECT
public:
    explicit TrashBin(QWidget *parent = nullptr);

signals:

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private:
    void printDefault(const QPaintEvent* event, QPainter& painter);
    void printDrop(const QPaintEvent* event, QPainter& painter);
    QPixmap trashIcon;
    QPixmap openedTrashIcon;
    std::atomic<bool> isDrag;

};

#endif // TRASHBIN_H
