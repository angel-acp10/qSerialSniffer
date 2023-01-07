#include "CommWorker.h"
#include "table/Fragment.h"

CommWorker::CommWorker(FragmentsModel *fragModel,
                       SettingsDialog *settings,
                       QObject *parent)
    : QObject{parent},
      m_fragModel(fragModel),
      m_settings(settings)
{
    m_tStamp0 = new TimeStamp(this);
    m_tStamp1 = new TimeStamp(this);
    m_ser = new SerialIO(this);
    m_cmd = new CommandManager(m_ser, m_tStamp0, m_tStamp1, this);
    m_timer = new QTimer(this);

    qRegisterMetaType<Fragment>();
    qRegisterMetaType<QList<Fragment>>();
    connect(m_timer, &QTimer::timeout, m_cmd->getAllQueue, &GetAllQueue::write);
    connect(m_cmd->getAllQueue, &GetAllQueue::received, m_fragModel, &FragmentsModel::appendFragments);
}

CommWorker::~CommWorker()
{
    delete m_timer;
    delete m_cmd;
    delete m_ser;
    delete m_tStamp0;
    delete m_tStamp1;
}

void CommWorker::moveChildrenToThread(QThread *thread)
{
    this->moveToThread(thread);
    for(QObject *child : this->children())
        child->moveToThread(thread);
    m_ser->moveChildrenToThread(thread);
    m_cmd->moveChildrenToThread(thread);
}

void CommWorker::play()
{
    m_ser->setBaudRate(460800);
    m_ser->setDataBits(QSerialPort::DataBits::Data8);
    m_ser->setParity(QSerialPort::Parity::NoParity);
    m_ser->setPortName(m_settings->getPort());
    m_ser->setStopBits(QSerialPort::StopBits::OneStop);
    m_ser->setTimeout(200);
    m_ser->open();
    m_cmd->initUart->write(115200,
                            InitUart::DataSize::DATASIZE_8bits,
                            InitUart::Parity::PARITY_NONE,
                            InitUart::Stop::STOP_1bit);
    m_timer->start(20);

    m_tStamp0->onPlayClicked();
    m_tStamp1->onPlayClicked();
}
void CommWorker::pause()
{
    m_tStamp0->onPauseClicked();
    m_tStamp1->onPauseClicked();

    m_timer->stop();
    m_cmd->deInitUart->write();
    m_ser->close();
}
