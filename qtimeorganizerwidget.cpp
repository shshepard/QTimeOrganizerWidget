#include "qtimeorganizerwidget.h"

QTimeOrganizerWidget::QTimeOrganizerWidget(QWidget *parent) : QWidget(parent)
{


    //create

    range_.dtBegin = new QDateTime(QDate::currentDate(),QTime(0,0));
    range_.dtEnd = new QDateTime(QDate::currentDate(),QTime(0,0));

    currentDate_ = new QDate(QDate::currentDate());
    currentTime_ = new QTime(0,0);

    setMouseTracking(true);

    //layouts

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(topHeader_,0,0,Qt::AlignTop);
    mainLayout->addWidget(scrollWidget_,1,0,Qt::AlignRight);
    mainLayout->setMargin(1);

    QHBoxLayout* topHeaderLayout = new QHBoxLayout(topHeader_);
    topHeaderLayout->addWidget(prevDayBtn_);
    topHeaderLayout->addWidget(dateEdit_);
    topHeaderLayout->addWidget(nextDayBtn_);
    topHeaderLayout->setMargin(1);

    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget_);
    scrollLayout->addWidget(scrollUpBtn_);
    scrollLayout->addStretch(1);
    scrollLayout->addWidget(scrollDownBtn_);
    scrollLayout->setMargin(0);

    //styleshit

    setStyleSheet("QToolButton { "
                  "background-color: transparent; "
                  "}; "
                  );

    //header widget propirties

    topHeader_->setGraphicsEffect(headerOpacityEffect_);

    //navigation buttons apperance

    QIcon icArrowBack = style()->standardIcon(QStyle::SP_ArrowBack);
    prevDayBtn_->setIcon(icArrowBack);

    QIcon icArrowForward = style()->standardIcon(QStyle::SP_ArrowForward);
    nextDayBtn_->setIcon(icArrowForward);

    scrollUpBtn_->setArrowType(Qt::UpArrow);
    scrollDownBtn_->setArrowType(Qt::DownArrow);

    scrollUpBtn_->setObjectName("btnScrollUp");
    scrollDownBtn_->setObjectName("btnScrollDown");

    //dateEdit

    dateEdit_->setAlignment(Qt::AlignLeft);
    dateEdit_->setDate(*currentDate_);
    dateEdit_->setCalendarPopup(true);
    dateEdit_->setDisplayFormat("MMMM dd (yyyy)");

    //connections

    connect(dateEdit_,&QDateEdit::dateChanged,
            this,&QTimeOrganizerWidget::setCurrentDate);

    connect(nextDayBtn_,&QToolButton::pressed,
            this,&QTimeOrganizerWidget::nextDate);

    connect(prevDayBtn_,&QToolButton::pressed,
            this,&QTimeOrganizerWidget::previousDate);

    connect(scrollUpBtn_,&QToolButton::pressed,
            this,&QTimeOrganizerWidget::scrollButtonPressed);
    connect(scrollDownBtn_,&QToolButton::pressed,
            this,&QTimeOrganizerWidget::scrollButtonPressed);

    connect(scrollUpBtn_,&QToolButton::released,
            this,&QTimeOrganizerWidget::scrollButtonReleased);
    connect(scrollDownBtn_,&QToolButton::released,
            this,&QTimeOrganizerWidget::scrollButtonReleased);

    connect(scrollTimer_,&QTimer::timeout,
            this,&QTimeOrganizerWidget::scrollTimeout);

}

QTimeOrganizerWidget::~QTimeOrganizerWidget()
{
    qDeleteAll(busyVector_.begin(),busyVector_.end());
    busyVector_.clear();

    delete currentDate_;
    delete currentTime_;
}

//METHODS///////////////////////////////////////////////////////////////////////

//public

    //set

void QTimeOrganizerWidget::setDrawingGrid(bool enabled)
{
    drawingGridEnabled_ = enabled;
    update();
}

void QTimeOrganizerWidget::setTracking(bool enabled)
{
    trackingEnabled_ = enabled;
}

void QTimeOrganizerWidget::setGridEnabled(bool enabled)
{
    gridEnabled_ = enabled;
    update();
}

void QTimeOrganizerWidget::setMinItemHeight(int h)
{
    minItemHeight_ = h;
    update();
}

void QTimeOrganizerWidget::setGrid(GridSeparation sep)
{
    gridSeparation_ = sep;
    update();
}

void QTimeOrganizerWidget::setHeaderVisible(bool visible)
{
    topHeader_->setVisible(visible);
    headerVisible_ = visible;
}

void QTimeOrganizerWidget::setHintsVisible(bool visible)
{
    hintsVisible_ = visible;
    update();
}

void QTimeOrganizerWidget::setScrolingMode(ScrollingMode mode)
{
    if (scrollingMode_ == OngoingScroll && mode == ScrollHours) {
        setTime(QTime(currentTime_->hour(),0));
    }

    scrollingMode_ = mode;
}

    //get

bool QTimeOrganizerWidget::gridEnabled() const
{
    return gridEnabled_;
}

bool QTimeOrganizerWidget::trackingEnabled() const
{
    return trackingEnabled_;
}

bool QTimeOrganizerWidget::drawingGridEnabled() const
{
    return drawingGridEnabled_;
}

int QTimeOrganizerWidget::minItemHeight() const
{
    return minItemHeight_;
}

QTimeOrganizerWidget::GridSeparation QTimeOrganizerWidget::gridSeparation() const
{
    return gridSeparation_;
}

int QTimeOrganizerWidget::hoursDisplayed() const
{
    return hoursDisplayed_;
}

QDate* QTimeOrganizerWidget::currentDate() const
{
    return currentDate_;
}

QTime* QTimeOrganizerWidget::currentTime() const
{
    return currentTime_;
}

bool QTimeOrganizerWidget::isHintsVisible() const
{
    return hintsVisible_;
}

QTimeOrganizerWidget::ScrollingMode QTimeOrganizerWidget::scrollingMode() const
{
    return scrollingMode_;
}

QDateTime* QTimeOrganizerWidget::dateTimeBegin() const
{
    return range_.dtBegin;
}

QDateTime* QTimeOrganizerWidget::dateTimeEnd() const
{
    return range_.dtEnd;
}

//private

void QTimeOrganizerWidget::checkSelection()
{
    if (selectionRect_.top() <= selectionRect_.bottom()) {
        selectDirection_ = Forward;
    } else {
        selectDirection_ = Reverse;
    }
}

void QTimeOrganizerWidget::updateTime()
{
    *range_.dtBegin = dateTimeUnderY(selectionRect_.top());
    *range_.dtEnd = dateTimeUnderY(selectionRect_.bottom());

    if (selectDirection_ == Forward)
        emit rangeChanged(*range_.dtBegin, *range_.dtEnd);
    else if (selectDirection_ == Reverse)
        emit rangeChanged(*range_.dtEnd, *range_.dtBegin);

}

QDateTime QTimeOrganizerWidget::dateTimeUnderY(int y)
{
    double dSec = static_cast<double>(y) * 3600 /
            static_cast<double>(itemHeight_);

    qint64 sec = static_cast<qint64>(round(dSec));
    return QDateTime(*currentDate_,*currentTime_).addSecs(sec);
}

int QTimeOrganizerWidget::dateTimeToY(const QDateTime &dt)
{
    QDateTime dtStart(*currentDate_, *currentTime_);
    qint64 sec = dtStart.secsTo(dt);
    double dY = static_cast<double>(sec) * static_cast<double>(itemHeight_)
            / 3600;

    int y = static_cast<int>(round(dY));
    return y;
}


void QTimeOrganizerWidget::setDate(const QDate &date)
{
    *currentDate_ = date;
    dateEdit_->blockSignals(true);
    dateEdit_->setDate(date);
    dateEdit_->blockSignals(false);

    if ( selectionRect_.top() != 0 && selectionRect_.bottom() != 0)
        updateTime();

    emit dateChanged(*currentDate_);
}

void QTimeOrganizerWidget::setTime(const QTime &time)
{
    *currentTime_ = time;
    updateYShift();
    update();
}

void QTimeOrganizerWidget::updateTimeShift()
{
    double dSec = static_cast<double>(yShift_) * 3600 /
            static_cast<double>(itemHeight_);

    qint64 sec = static_cast<qint64>(round(dSec));
    *currentTime_ = QTime(currentTime_->hour(),0,0).addSecs(sec);
}

void QTimeOrganizerWidget::updateYShift()
{
    qint64 sec = currentTime_->minute() * 60 + currentTime_->second();

    double dY = static_cast<double>(sec) * static_cast<double>(itemHeight_)
            / 3600;

    int y = static_cast<int>(round(dY));

    yShift_ = y;
}

void QTimeOrganizerWidget::scrollUp()
{
    if (scrollingMode_ == OngoingScroll) {

        if ( abs(yShift_) == 0 ) {

            yShift_ = itemHeight_ - 1;

            if (currentTime_->hour() == 0) {

                setDate(currentDate_->addDays(-1));
                currentTime_->setHMS(23,0,0);
            } else {
                currentTime_->setHMS(currentTime_->hour() - 1,0,0);
            }

        } else {
            yShift_--;
        }

        updateTimeShift();
    } else if (scrollingMode_ == ScrollHours) {
        nextHour();
    }

    updateTime();
    update();

}

void QTimeOrganizerWidget::scrollDown()
{

    if (scrollingMode_ == OngoingScroll) {

        if ( yShift_ == itemHeight_  - 1) {

            yShift_ = 0;

            if (currentTime_->hour() == 23) {
                setDate(currentDate_->addDays(1));
                currentTime_->setHMS(0,0,0);
            } else {
                currentTime_->setHMS(currentTime_->hour() + 1,0,0);
            }

        } else {
            yShift_++;
        }

        updateTimeShift();

    } else if (scrollingMode_ == ScrollHours) {
        previousHour();
    }

    updateTime();
    update();
}

int QTimeOrganizerWidget::findCloseToGrid(int point)
{
    int diff = height();
    int result = point;

    foreach (int y, gridVector_) {
        if (abs(y - point) < diff) {
            diff = abs(y - point);
            result = y;
        }
    }

    return result;
}

//EVENTS////////////////////////////////////////////////////////////////////////

void QTimeOrganizerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed_) {

        int y = event->y();

        if (y > height()) {
            y = height();
        } else if (y < 0) {
            y = 0;
        }

        if (dragSelection_ ) {

            if (dragDirection_ == UpDrag) {

                if (event->y() < selectionRect_.bottom()) {
                    if (gridEnabled_) {
                        selectionRect_.setTop(findCloseToGrid(y));
                    } else {
                        selectionRect_.setTop(y);
                    }
                } else {
                    selectionRect_.setTop(selectionRect_.bottom());
                }

            } else if (dragDirection_ == DownDrag) {

                if (event->y() > selectionRect_.top()) {
                    if (gridEnabled_) {
                        selectionRect_.setBottom(findCloseToGrid(y));
                    } else {
                        selectionRect_.setBottom(y);
                    }
                } else {
                    selectionRect_.setTop(selectionRect_.bottom());
                }

            } else if (dragDirection_ == NoDragDirection) {

            }

        } else {

            if (gridEnabled_) {
                selectionRect_.setBottom(findCloseToGrid(y));
            } else {
                selectionRect_.setBottom(y);
            }

            hintPointEnd_ = QPoint(event->x(),selectionRect_.bottom());
            checkSelection();
        }

        if (trackingEnabled_)
            updateTime();

        update();

    } else {

        if (rectDragBegin_.contains(event->pos()) ||
                rectDragEnd_.contains(event->pos())) {
            setCursor(QCursor(Qt::SizeVerCursor));
        } else {
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void QTimeOrganizerWidget::mousePressEvent(QMouseEvent *event)
{
    if (headerVisible_ ) {

        if (dateEdit_->hasFocus())
            dateEdit_->clearFocus();
        headerOpacityEffect_->setOpacity(0.3);

    }

    mousePressed_ = true;
    previousMouseY_ = event->y();
    currentMouseY_ = event->y();

    if (selectionRect_.contains(event->pos())) {

        dragSelection_ = true;

        selectDirection_ = Forward;
        dragDirection_ = NoDragDirection;

        if (selectionRect_.top() > selectionRect_.bottom()) {
            int buf = selectionRect_.top();
            selectionRect_.setTop(selectionRect_.bottom());
            selectionRect_.setBottom(buf);
        }

        if (rectDragBegin_.contains(event->pos())) {
            dragDirection_ = UpDrag;
        } else if (rectDragEnd_.contains(event->pos())) {
            dragDirection_ = DownDrag;
        }

    } else {

        if (event->y() < 0) {
            selectionRect_.setTop(0);
        } else if (event->y() > height()) {
            selectionRect_.setTop(height());
        } else {

            if (gridEnabled_) {
                selectionRect_.setTop(findCloseToGrid(event->y()));
            } else {
                selectionRect_.setTop(event->y());
            }
        }

        selectionRect_.setBottom(selectionRect_.top());
    }

    update();
}

void QTimeOrganizerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed_ = false;

    if (!dragSelection_) {

        if (event->y() < 0) {
            selectionRect_.setBottom(0);
        } else if (event->y() > height()) {
            selectionRect_.setBottom(height());
        }

        if (gridEnabled_) {
            selectionRect_.setBottom(findCloseToGrid(event->y()));
        } else {
            selectionRect_.setBottom(event->y());
        }
    }

    dragSelection_ = false;

    if (headerVisible_) {

        if (topHeader_->isHidden())
            topHeader_->show();

        headerOpacityEffect_->setOpacity(0.7);
    }


    updateTime();
    update();
}

void QTimeOrganizerWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        scrollUp();
    else
        scrollDown();
}

void QTimeOrganizerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawRect(QRect(0,0,width(),height()));

    //calulate item height
    int newItemHeight;

    if ( (height() / hoursDisplayed_) < minItemHeight_) {
        newItemHeight = minItemHeight_;
    } else {
        newItemHeight = height() / hoursDisplayed_;
    }

    if (gridEnabled_) {
        int div = static_cast<int>(gridSeparation_);

        while( newItemHeight % div )
            newItemHeight++;

        gridVector_.clear();
    }

    if (newItemHeight != itemHeight_) {
        itemHeight_ = newItemHeight;
        updateYShift();
        updateTimeShift();
        updateTime();

    }

    //set top left point
    QPoint topLeft(0, 0 - yShift_);

    //draw hours
    painter.save();

    int hour = currentTime_->hour();
    int magicNumber = 3;
    int itemsToDraw = height() / itemHeight_ + magicNumber;
    int days = hour == 0 ? 0 : 1;

    while(itemsToDraw) {

        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setColor(palette().color(QPalette::Midlight));
        pen.setWidth(1);

        painter.setPen(pen);
        painter.setBrush(palette().color(QPalette::Base));

        QRect rectItem(topLeft,QSize(width(),itemHeight_));
        painter.drawRect(rectItem);

        //grid//////

        if (gridEnabled_) {

            pen.setStyle(Qt::DotLine);
            pen.setColor(palette().color(QPalette::Midlight));
            painter.setPen(pen);

            int div = static_cast<int>(gridSeparation_);
            int step = rectItem.height() / div;
            int gridY = rectItem.topLeft().y();

            gridVector_.push_back(gridY);

            for(int i = 0; i < div - 1; ++i) {

                gridY += step;

                if (drawingGridEnabled_) {

                    QPoint p1, p2;
                    p1.setX(0);
                    p1.setY(gridY);

                    p2.setX(width());
                    p2.setY(gridY);

                    painter.drawLine(p1,p2);
                }

                gridVector_.push_back(gridY);
            }
        } //if grid enabled

        //Draw hour//////

        pen.setStyle(Qt::SolidLine);
        pen.setColor(palette().color(QPalette::ButtonText));
        painter.setPen(pen);
        painter.drawText(rectItem,QString("%1:00").arg(hour,2,10,QChar('0')),
                     QTextOption(Qt::AlignHCenter | Qt::AlignTop)
                     );

        //Draw date/////
        painter.save();

        if (hour == 0) {

            qDebug() << this << "h: " << hour <<
                        "d: " << days <<
                        "itd: " << itemsToDraw;

            if (!rectItem.intersects(topHeader_->rect())) {

                pen.setColor(palette().color(QPalette::Mid));
                painter.setPen(pen);

                QFont font;
                font.setPointSize(font.pointSize() + 1);
                painter.setFont(font);

                QDate date(currentDate_->addDays(days));

                painter.drawText(rectItem,
                                 date.toString(" MMMM dd"),
                                 QTextOption(Qt::AlignLeft | Qt::AlignTop)
                                );
            }

            days++;
        }

        painter.restore();

        topLeft.setY(topLeft.y() + itemHeight_);

        hour += 1;
        hour %= 24;

        itemsToDraw--;
    } // big while

    painter.restore();

    //draw busy area//////

    painter.save();

    QBrush busyBrush; // =)
    busyBrush.setColor(QColor(Qt::red));
    busyBrush.setStyle(Qt::SolidPattern);

    QPen busyPen;
    busyPen.setColor(palette().color(QPalette::Text));

    foreach (TextRange* textRange, busyVector_) {

        int yBusyBegin = dateTimeToY(*textRange->range.dtBegin);
        int yBusyEnd = dateTimeToY(*textRange->range.dtEnd);

        if ( (yBusyBegin <= 0 && yBusyEnd >= 0) ||
            (yBusyBegin >= 0 && yBusyEnd <= height()) ||
            (yBusyBegin <= height() && yBusyEnd >= height()) ) {

            painter.setBrush(busyBrush);
            painter.setPen(Qt::NoPen);
            painter.setOpacity(0.15);

            QRect rectBusy(QPoint(0,yBusyBegin),
                           QSize(width(),yBusyEnd - yBusyBegin));

            painter.drawRect(rectBusy);

            //

            QRect rectBusyText(QPoint(0, yBusyBegin ),
                               QSize(yBusyEnd - yBusyBegin,itemHeight_ / 2));

            painter.setPen(busyPen);
            painter.setOpacity(0.7);

            QTransform transform;
            transform.translate(rectBusyText.topLeft().x() + rectBusyText.height() + yBusyBegin,
                                rectBusyText.topLeft().y());
            transform.rotate(90);

            painter.setTransform(transform);

            QFont font;
            font.setPointSize(font.pointSize() + 2);
            painter.setFont(font);

            painter.drawText(rectBusyText,Qt::AlignCenter,textRange->text);

            painter.resetTransform();
        }
    }

    painter.restore();

    //draw selection//////

    if(selectionRect_.top() != selectionRect_.bottom()) {
        painter.save();

        if (mousePressed_)
            painter.setOpacity(0.35);
        else
            painter.setOpacity(0.25);

        painter.setBrush(palette().color(QPalette::Highlight));
        painter.setPen(Qt::NoPen);

        selectionRect_.setWidth(width());
        painter.drawRect(selectionRect_);

        painter.restore();
    }

    //draw selection drag rects
    if (selectionRect_.top() != selectionRect_.bottom()) {

        painter.save();

        painter.setBrush(palette().color(QPalette::AlternateBase));
        painter.setPen(Qt::NoPen);
        painter.setOpacity(0.9);

        int dragRectH = height() / 80;
        int dragRectW = width() / 5;
        int dragRectLeft = width() * 2 / 5;


        if (selectDirection_ == Forward) {

            rectDragBegin_ = QRect(dragRectLeft, selectionRect_.top(),dragRectW,dragRectH);
            rectDragEnd_ = QRect(dragRectLeft,selectionRect_.bottom() - dragRectH,dragRectW,dragRectH);

        } else if (selectDirection_ == Reverse) {

            rectDragEnd_ = QRect(dragRectLeft, selectionRect_.top() - dragRectH,dragRectW,dragRectH);
            rectDragBegin_ = QRect(dragRectLeft,selectionRect_.bottom(),dragRectW,dragRectH);
        }

        painter.drawRoundedRect(rectDragBegin_,5.0,5.0);
        painter.drawRoundedRect(rectDragEnd_,5.0,5.0);

        painter.setPen(Qt::SolidPattern);


        QFont font;

        if (dragSelection_ && dragDirection_ == UpDrag) {
            font.setBold(true);
            painter.setFont(font);
        }
        painter.drawText(rectDragBegin_,tr("+"),QTextOption(Qt::AlignCenter));

        font.setBold(false);
        painter.setFont(font);

        if (dragSelection_ && dragDirection_ == DownDrag) {
            font.setBold(true);
            painter.setFont(font);
        }

        painter.drawText(rectDragEnd_,tr("+"),QTextOption(Qt::AlignCenter));

        painter.restore();
    }

    //draw hints

    if (hintsVisible_) {

        painter.save();

        if (showHint_ && (selectionRect_.top() != selectionRect_.bottom()) ) {

            //hint begin

            QDateTime dtHintBegin;
            hintPointBegin_.setX(0);
            int hintFlags = static_cast<int>(Qt::AlignRight | Qt::AlignTop);

            if (selectDirection_ == Forward) {
                hintPointBegin_.setY(selectionRect_.top());
                dtHintBegin = dateTimeUnderY(selectionRect_.top());
            } else if (selectDirection_ == Reverse) {
                hintPointBegin_.setY(selectionRect_.bottom());
                dtHintBegin = dateTimeUnderY(selectionRect_.bottom());
            }

            QRect rectHintBegin(hintPointBegin_,QSize(width(),itemHeight_ ));
            painter.drawText(rectHintBegin,
                             hintFlags,
                             dtHintBegin.time().toString(" hh:mm:ss "));

            QDateTime dtHintEnd;

            if (selectionRect_.top() != selectionRect_.bottom()) {

                //hint end
                hintPointEnd_.setX(0);
                hintFlags = static_cast<int>(Qt::AlignRight | Qt::AlignBottom);

                if (selectDirection_ == Forward) {
                    hintPointEnd_.setY(selectionRect_.bottom() - itemHeight_);
                    dtHintEnd = dateTimeUnderY(selectionRect_.bottom());
                } else if (selectDirection_ == Reverse) {
                    hintPointEnd_.setY(selectionRect_.top() - itemHeight_);
                    dtHintEnd = dateTimeUnderY(selectionRect_.top());
                }

                QRect rectHintEnd(hintPointEnd_,QSize(width(),itemHeight_));
                painter.drawText(rectHintEnd,
                                 hintFlags,
                                 dtHintEnd.time().toString(" hh:mm:ss "));
            }

            int lineX = width() / 5;

            int lineP1y = selectionRect_.top();
            int lineP2y = selectionRect_.bottom();

            if (selectDirection_ == Forward) {
                lineP2y--;
            } else if (selectDirection_ == Reverse) {
                lineP1y--;
            }

            QLine line(lineX, lineP1y,
                  lineX, lineP2y);

            painter.drawLine(line);

            QRect rectLineText(line.x1(),
                               line.y1(), line.y2() - line.y1(), width() / 8);


            painter.translate(rectLineText.topLeft().x() + rectLineText.height() + selectionRect_.top(),
                              rectLineText.topLeft().y() - line.x1());

            painter.rotate(90);

            QString lineText;

            qint64 secs = dtHintBegin.secsTo(dtHintEnd);

            int days = secs / (60*60*24);
            int hours = (secs % (60*60*24)) / 3600;
            int minutes = ( (secs % (60*60*24)) % 3600 ) / 60;
            int seconds = ( (secs % (60*60*24)) % 3600 ) % 60;

            if (days) {
                lineText.append(QString::number(days) + tr("d "));
            }

            QTime lineTime(hours,minutes,seconds);

            lineText.append(lineTime.toString("hh:mm:ss"));

            QFont font;
            font.setPointSize(font.pointSize() + 1);
            font.setBold(true);
            painter.setFont(font);

            painter.drawText(rectLineText,Qt::AlignCenter,lineText);

        }
        painter.restore();
    }

    event->accept();
}

//SLOTS/////////////////////////////////////////////////////////////////////////

    //private

void QTimeOrganizerWidget::scrollTimeout()
{
    scrollTimer_->stop();

    if (scrollingDirection_ == ScrollingUp) {
        scrollUp();
    } else if (scrollingDirection_ == ScrollingDown) {
        scrollDown();
    }

    int speed = scrollingSpeed_;

    if (scrollingMode_ == OngoingScroll)
        speed /= itemHeight_;

    scrollTimer_->start(speed);
}

void QTimeOrganizerWidget::scrollButtonPressed()
{
    if (sender()->objectName() == "btnScrollUp") {
        scrollingDirection_ = ScrollingUp;
    } else if (sender()->objectName() == "btnScrollDown") {
        scrollingDirection_ = ScrollingDown;
    }

    scrollTimer_->start(0);
}

void QTimeOrganizerWidget::scrollButtonReleased()
{
    scrollTimer_->stop();
}

    //public

void QTimeOrganizerWidget::clearSelection()
{
    selectionRect_.setTop(0);
    selectionRect_.setBottom(0);

    rectDragBegin_ = QRect(-1,-1,0,0);
    rectDragEnd_ = QRect(-1,-1,0,0);
    updateTime();
    update();
}

void QTimeOrganizerWidget::setHoursDisplayed(int count)
{
    if (count > 0 && count <= 36) {
        hoursDisplayed_ = count;
        update();
    } else {
        qWarning() << "Can't set hours count : " << count;
    }

}

void QTimeOrganizerWidget::clearBusyRange()
{
    foreach (TextRange* tRange, busyVector_) {
        delete tRange->range.dtBegin;
        delete tRange->range.dtEnd;
    }

    qDeleteAll(busyVector_.begin(),busyVector_.end());
    busyVector_.clear();
    update();
}

void QTimeOrganizerWidget::addBusyRange(const QDateTime &dtBegin,
                                        const QDateTime &dtEnd,
                                        const QString &text)
{
    TextRange* textRange = new TextRange();
    textRange->range.dtBegin = new QDateTime(dtBegin);
    textRange->range.dtEnd = new QDateTime(dtEnd);
    textRange->text = text;

    busyVector_.push_back(textRange);
    update();

}

void QTimeOrganizerWidget::setSelectedRange(const QDateTime &dtBegin,
                                    const QDateTime &dtEnd)
{
    int yBegin = dateTimeToY(dtBegin);
    int yEnd = dateTimeToY(dtEnd);

    if ( (yBegin >= 0) && (yEnd < height()) && (yBegin < yEnd) ) {
        selectionRect_.setTop(yBegin);
        selectionRect_.setBottom(yEnd);
        checkSelection();
        update();
    }

}

void QTimeOrganizerWidget::setCurrentDate(const QDate &date)
{
    setDate(date);
    update();
}

void QTimeOrganizerWidget::nextDate()
{
    setDate(currentDate_->addDays(1));
    update();
}

void QTimeOrganizerWidget::previousDate()
{
    setDate(currentDate_->addDays(-1));
    update();
}

void QTimeOrganizerWidget::setCurrentHour(int hour)
{
    if (hour >= 0 && hour < 24) {
        setTime(QTime(hour,0));
        updateTime();
        update();
    } else {
        qWarning() << "There is no hour: " << hour;
    }
}

void QTimeOrganizerWidget::nextHour()
{
    if (currentTime_->hour() == 23) {
        currentTime_->setHMS(0,0,0);
        setDate(currentDate_->addDays(1));
    } else {
        currentTime_->setHMS(currentTime_->hour() + 1,0,0);
    }
}

void QTimeOrganizerWidget::previousHour()
{
    if (currentTime_->hour() == 0) {
        currentTime_->setHMS(23,0,0);
        setDate(currentDate_->addDays(-1));
    } else {
        currentTime_->setHMS(currentTime_->hour() - 1,0,0);
    }
}
