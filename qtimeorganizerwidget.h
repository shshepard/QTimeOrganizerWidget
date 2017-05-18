#ifndef QTIMEORGANIZERWIDGET_H
#define QTIMEORGANIZERWIDGET_H

#include <math.h>
//
#include <QWidget>
#include <QListView>
#include <QAbstractListModel>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QDate>
#include <QTime>
#include <QPainter>
#include <QDebug>
#include <QCalendarWidget>
#include <QDateEdit>
#include <QLayout>
#include <QGraphicsOpacityEffect>
#include <QToolButton>
#include <QThread>
#include <QTimer>
#include <QPixmap>
//

class QTimeOrganizerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QTimeOrganizerWidget(QWidget *parent = 0);
    ~QTimeOrganizerWidget();

    enum GridSeparation {

        Hour = 1,
        Minute30 = 2,
        Minute15 = 4,
        Minute10 = 6,
        Minute5 = 12,
        Minute1 = 60,
        Second30 = 120

    };

    enum ScrollingMode {

        ScrollHours,
        OngoingScroll

    };

    enum ScrollingDirection {

        ScrollingUp,
        ScrollingDown

    };

    enum DragDirection {
        UpDrag,
        DownDrag,
        NoDragDirection
    };

    void setGridEnabled(bool enabled);
    void setTracking(bool enabled);
    void setDrawingGrid(bool enabled);
    void setMinItemHeight(int h);
    void setGrid(GridSeparation sep);
    void setHeaderVisible(bool visible);
    void setHintsVisible(bool visible);
    void setScrolingMode(ScrollingMode mode);

    bool gridEnabled() const;
    bool trackingEnabled() const;
    bool drawingGridEnabled() const;
    bool isHintsVisible() const;

    int minItemHeight() const;
    GridSeparation gridSeparation() const;
    int hoursDisplayed() const;
    QDate* currentDate() const;
    QTime* currentTime() const;
    QDateTime* dateTimeBegin() const;
    QDateTime* dateTimeEnd() const;
    ScrollingMode scrollingMode() const;

signals:

    void rangeChanged(const QDateTime&, const QDateTime&);
    void dateChanged(const QDate&);

public slots:

    void setSelectedRange(const QDateTime& dtBegin,
                  const QDateTime& dtEnd);

    void setHoursDisplayed(int count);

    void setCurrentDate(const QDate& date);

    void setCurrentHour(int hour);

    void addBusyRange(const QDateTime& dtBegin,
                      const QDateTime& dtEnd,
                      const QString& text = QString());

    void clearBusyRange();

    void clearSelection();

    void nextDate();

    void previousDate();

    void nextHour();

    void previousHour();

protected:

    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private slots:

    void scrollTimeout();
    void scrollButtonPressed();
    void scrollButtonReleased();

private:

    enum SelectDirection { Forward, Reverse };

    typedef struct {

        QDateTime* dtBegin;
        QDateTime* dtEnd;

    } Range;

    typedef struct {

        Range range;
        QString text;

    } TextRange;

    QTimer* scrollTimer_ = { new QTimer(this)};

    //header widgets

    QWidget* topHeader_ = { new QWidget(this) };
    QDateEdit* dateEdit_= { new QDateEdit(this) };
    QToolButton* nextDayBtn_ = { new QToolButton(this) };
    QToolButton* prevDayBtn_ = { new QToolButton(this) };

    QWidget* scrollWidget_ = { new QWidget(this) };
    QToolButton* scrollUpBtn_ = { new QToolButton(this) };
    QToolButton* scrollDownBtn_ = { new QToolButton(this) };

    QGraphicsOpacityEffect * headerOpacityEffect_ = { new QGraphicsOpacityEffect(this) };

    //selection drag rects

    QRect rectDragBegin_;
    QRect rectDragEnd_;

    //where hit points draw

    QPoint hintPointBegin_;
    QPoint hintPointEnd_;

    //currentDate_ contains current date - obvious =)

    QDate* currentDate_;

    //currentTime_ - contains time shift relate to "y" coordinates shift

    QTime* currentTime_;

    //current dateTime selection

    //scrollign speed

    int scrollingSpeed_ = 100;

    // "y" coordinates shift
    int yShift_ = 0;

    //one item heihgt
    int itemHeight_;

    //minimum item height
    int minItemHeight_ = 10;

    //limit of hours displayed
    int hoursDisplayed_ = 12;

    //selection rect "y" coordinates

    QRect selectionRect_;

    int previousMouseY_;
    int currentMouseY_;

    //bool properties
    bool gridEnabled_ = true;
    bool trackingEnabled_ = false;
    bool drawingGridEnabled_ = false;
    bool mousePressed_ = false;
    bool hintsVisible_ = true;
    bool headerVisible_ = true;
    bool showHint_ = true;
    bool dragSelection_ = false;

    //grid properties
    SelectDirection selectDirection_;
    GridSeparation gridSeparation_ = Minute15;
    ScrollingMode scrollingMode_ = ScrollHours;
    ScrollingDirection scrollingDirection_;
    DragDirection dragDirection_;

    //grid "y" coordinates
    QVector<int> gridVector_;

    //contains busy date-time intervals
    QVector<TextRange*> busyVector_;

    //selected range
    Range range_;

    //scrolling
    void scrollUp();
    void scrollDown();

    //check selection rect "y" coordinates and set select direction
    void checkSelection();

    //setting current date and time
    void setDate(const QDate& date);
    void setTime(const QTime& time);

    //get dateTime relate to "y" coordinate, current date and current time shift
    QDateTime dateTimeUnderY(int y);

    //same but vice versa =)
    int dateTimeToY(const QDateTime& dt);

    //find closest to "point" grid "y" coordinate
    int findCloseToGrid(int point);

    //calculate selection range and emit "rangeChanged" signal
    void updateTime();

    //set time shift relate to "y" coordinate
    void updateTimeShift();

    //set "y" coordinate shift relate to time
    void updateYShift();

};

#endif // QTIMEORGANIZERWIDGET_H
