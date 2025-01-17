#include "lpen.h"

LPen::LPen(QVector<QSharedPointer<LPen> > *pens, QVector<LColor> *colors, int cidx, QString name, LPen::Type type) {
    m_colorIndex = cidx;
    m_name = name;
    m_type = type;
    m_pens = pens;
    m_colors = colors;
}

LPen::LPen(QVector<QSharedPointer<LPen> > *pens, QVector<LColor> *colors, QSharedPointer<LPen> copy) {
    m_colorIndex = copy->m_colorIndex;
    m_name = copy->m_name;
    m_type = copy->m_type;
    m_bpp = copy->m_bpp;
    m_pens = pens;
    m_colors = colors;
}

LPen::LPen(QVector<QSharedPointer<LPen> > *pens, QVector<LColor> *colors, int cidx, QString name, LPen::Type type, QVector3D bpp) {
    m_colorIndex = cidx;
    m_name = name;
    m_type = type;
    m_pens = pens;
    m_bpp = bpp;
    m_colors = colors;
}

QWidget* LPen::CreateUI(QColor col, int width,int xx,int yy,QVector<LColor>& list) {

    QWidget* widget = nullptr;
    if (m_type == FixedSingle)
        widget = createButton(col, m_colorIndex, width);

    if (m_type == FixedSingleNumbers)
        widget = createButtonNumber(col, m_colorIndex, width);

    if (m_type == SingleSelect)
        widget = createButtonSelect(col, m_colorIndex, width);

    if (m_type == Dropdown || m_type == DropDownExceptAlreadySelected)
        widget = createComboBox(col, width, list);

    if (m_type == DisplayAllExceptAlreadySelected || m_type == DisplayAll)
        widget = createGrid(col, width,list);


    if (widget == nullptr) {
        qDebug() << "Error in LPen::CreateUI : widget is zero. Should not happen!";
    }

    return widget;

    //m_buttonsEdit.append(b);

}


QWidget *LPen::createButton(QColor col, int index, int width) {
    QPushButton *b = new QPushButton();
    QPalette p;
    b->setFlat(true);
    if (!col.isValid())
        col = QColor(0,0,0,255);
    QPixmap pm = Util::CreateColorIcon(col,width);
    b->setAutoFillBackground(true);
    p.setBrush(b->backgroundRole(), QBrush(pm));
    b->setMaximumWidth(width);
    b->setMinimumWidth(width);
    b->setMaximumHeight(width);
    b->setMinimumHeight(width);

    QColor c2(0,0,0,255);
    if (col.red()+col.green()+col.blue()<(127*3))
        c2 = QColor(255,255,255,255);
    p.setColor(b->foregroundRole(),c2);
    b->setPalette(p);
    if (index == Data::data.currentColor)
        b->setText("X");

//    b->setStyleSheet("QLabel { color : " + c2.name() +"; }");
    QObject::connect( b, &QPushButton::clicked,  [=](){
        Data::data.currentColorType = m_dataType;
//        qDebug() << "Setting "<<Data::data.currentColorType;
        handleButtonEdit(index,b);
    } );
    return b;
}

QWidget *LPen::createButtonNumber(QColor col, int index, int width) {
    QPushButton *b = new QPushButton();
    b->setMaximumWidth(width);
    b->setMinimumWidth(width);
    b->setMaximumHeight(width);
    b->setMinimumHeight(width);
//    b->setFlat(true);

    /*QPalette p;

    if (!col.isValid())
        col = QColor(0,0,0,255);
    QPixmap pm = Util::CreateColorIcon(col,width);
    b->setAutoFillBackground(true);
    p.setBrush(b->backgroundRole(), QBrush(pm));

    QColor c2(0,0,0,255);
    if (col.red()+col.green()+col.blue()<(127*3))
        c2 = QColor(255,255,255,255);
    p.setColor(b->foregroundRole(),c2);
    b->setPalette(p);
    */
    b->setText(QString::number(index));
    if (index == Data::data.currentColor)
        b->setText("["+QString::number(index)+"]");

//    b->setStyleSheet("QLabel { color : " + c2.name() +"; }");
    QObject::connect( b, &QPushButton::clicked,  [=](){
        Data::data.currentColorType = m_dataType;
//        qDebug() << "Setting "<<Data::data.currentColorType;
        handleButtonEdit(index,b);

    } );
    return b;
}

QWidget *LPen::createButtonSelect(QColor col, int index, int width)
{
#ifndef CLI_VERSION

    QWidget* w = new QWidget();
    QGridLayout* ly = new QGridLayout();



    ly->addWidget(createButton(col,index,width/1.3),0,0);

    QPushButton* palSelect = new QPushButton("..");
    int w2= width/2;
    palSelect->setMaximumSize(w2,w2);
    ly->addWidget(createButton(col,index,width),0,0);
    ly->addWidget(palSelect,0,1);
    ly->setContentsMargins(QMargins(1,1,1,1));


    QObject::connect( palSelect, &QPushButton::clicked,  [=](){
        // Implement!
        DialogColorSelect* dc = new DialogColorSelect();
        dc->Init(LColor(col,""),m_bpp);
        dc->exec();
        if (dc->m_ok) {
           ((*m_colors)[index]).color = dc->m_color.color;
            Data::data.UpdatePens();
        }
    } );


    w->setLayout(ly);

    return w;
#else
    return nullptr;
#endif
}

QWidget *LPen::createComboBox(QColor col, int width, QVector<LColor> &list)
{
    QGridLayout* ly = new QGridLayout();
    ly->setVerticalSpacing(0);
    QWidget* btn = createButton(col,m_colorIndex,width);
    m_dataType = 1;
    QComboBox *b = new QComboBox();
    if (m_type == Dropdown || m_type == DisplayAll)
        FillComboBox(b, list);
    else
        FillComboBoxRestricted(b,list);

//    qDebug() << "Setting : "<<m_colorIndex;
    b->setCurrentIndex(m_colorIndex);

    QObject::connect( b, &QComboBox::currentTextChanged,  [=](){
        Data::data.currentColor = b->currentData(Qt::UserRole).toInt();
        Data::data.currentIsColor=true;
        m_colorIndex = Data::data.currentColor;


        Data::data.UpdatePens();

    } );

    ly->addWidget(btn,0,0);

    QGridLayout* ly_small = new QGridLayout();
    ly_small->setVerticalSpacing(0);

    if (m_name!="") {
        ly_small->addWidget(new QLabel(m_name),0,0);
        ly_small->addItem(new QSpacerItem(0,6,QSizePolicy::Fixed,QSizePolicy::Fixed),1,0);
    }
    ly_small->addWidget(b,2,0);

    QWidget* w_small = new QWidget();
    w_small->setLayout(ly_small);
//    w_small->setMaximumSize(1000,width);

    ly->addWidget(w_small,0,1);

    QWidget* dummy = new QWidget();
    dummy->setLayout(ly);
  //  ly->setMargin(2);
    ly->setContentsMargins(QMargins(2,2,2,2));
//    ly_small->setMargin(2);
    ly_small->setContentsMargins(QMargins(2,2,2,2));
//    dummy->setMaximumSize(1000,width*1);
    return dummy;

}

QWidget *LPen::createGrid(QColor col, int width, QVector<LColor> &list)
{
    QGridLayout* ly = new QGridLayout();
    int j=0;
    int x = 0;
    bool filled = false;
    ly->addWidget(new QLabel(m_name),0,0);
    for (int i=0;i<list.count();i++) {
        bool ok=true;

        if (m_type==DisplayAllExceptAlreadySelected) {
            for (int j=0;j<m_pens->count();j++)
                if (m_pens->at(j)->m_isHidden==false)
                if (m_pens->at(j)->m_colorIndex==i && m_pens->at(j)!=this)
                    ok = false;

        }
//        qDebug() << "REST" <<m_restricted;
        if (m_restricted.contains(i))
            ok = false;

        if (ok) {
            ly->addWidget(createButton(list[i&m_and].color,i,width),j+1,x);
            if (x>=1) filled = true;

        }
        j++;
        if (j==8) {
            j=0;x++;
        }

    }
    if (!filled) {
        QWidget* dummy = new QWidget();
        dummy->setMaximumSize(width,width);
        ly->addWidget(dummy,0,1);
    }

    QWidget* w = new QWidget();
    w->setLayout(ly);
    return w;
}

void LPen::FillComboBox(QComboBox *cmb, QVector<LColor> &list)
{
    cmb->clear();
    for (int i=0;i<list.count();i++) {
        QPixmap pixmap(16,16);
        pixmap.fill(list[i].color);
        QIcon icon(pixmap);
        if (!m_restricted.contains(i))
            cmb->addItem(icon,Util::numToHex(i),i);


    }
}

void LPen::FillComboBoxRestricted(QComboBox *cmb, QVector<LColor> &list)
{
    cmb->clear();
    for (int i=0;i<list.count();i++) {
        bool ok = true;
        for (int j=0;j<m_pens->count();j++)
            if (m_pens->at(j)->m_isHidden==false)
                if (m_pens->at(j)->m_colorIndex==i && m_pens->at(j)!=this)
                ok = false; // Already used, restricted

        if (m_restricted.contains(i))
            ok = false;

        if (ok) {
            QPixmap pixmap(16,16);
            pixmap.fill(list[i].color);
            QIcon icon(pixmap);
            cmb->addItem(icon,Util::numToHex(i),i);
        }
    }
}

void LPen::handleButtonEdit(int val, QPushButton *btn)
{
    /*    for (int i=0;i<m_buttonsEdit.count();i++)
            m_buttonsEdit[i]->setText("");
        //if (data<m_buttonsEdit.count())
    //    qDebug() << data;
            m_buttonsEdit[data]->setText("X");*/
    Data::data.currentColor = val;
//    Data::data.currentColorType = 0;

    Data::data.currentIsColor=true;

    Data::data.UpdatePens();
//    qDebug() << "Setting color "<< val;
    //        SetMulticolor(3,val);
    QPoint p = (QCursor::pos() - btn->mapToGlobal(QPoint(0,0)));
    QPointF fp = QPointF(p.x()/(float)btn->width(),
                         p.y()/(float)btn->height());
    //    qDebug() << (QCursor::pos() - btn->mapToGlobal(QPoint(0,0))) << btn->mapToGlobal(QPoint(0,0))  <<m_buttonsEdit[data]->cursor().pos();
    //    qDebug() <<fp;
    // Select alternative colour
    /*if (!m_list[val].ignoreAltColour)
        if (m_list[val].m_altColour!=-1)
            if (1-fp.y()<fp.x())
                Data::data.currentColor = m_list[val].m_altColour;
*/
  //  Data::data.currentColor = m_colorIndex;//  m_list[val].m_altColour;
    Data::data.currentIsColor=true;

}

int LPen::Get() {
    return m_colorIndex;
}

