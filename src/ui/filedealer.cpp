#include "filedealer.h"

#include <QMenu>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

FileDealer::FileDealer( QObject* parent ) :
    QObject( parent ),
    mMenu( 0 ) {

    mRecentFileActions.reserve( MaxRecentFiles );

    for( int i = 0; i < MaxRecentFiles; i++ ) {
        mRecentFileActions.push_back( 0 );
    }

    setType( ImagesType );
}

void FileDealer::setType( FileType type ) {
    mType = type;

    switch( mType ) {
        case ImagesType: {
            mLastDirectory = "lastDirectoryImages";
            mSaveTitle = tr( "Save image" );
            mOpenTitle = tr( "Open image" );
            mIOSelection = tr( "Images (*.jpg *.jpeg *.JPG *.JPEG *.png *.PNG *.tif *.TIF *.tiff *.TIFF)" );
            mSettingsRecent = "recentImages";
            mDefaultSuffix = "jpg";
            mPossibleSuffixes << "jpg" << "jpeg" << "png" << "tif" << "tiff";
            mUpdateRecentOnSave = false;
        }
        break;

        case PresetsType: {
            mLastDirectory = "lastDirectoryPresets";
            mSaveTitle = tr( "Save presets" );
            mOpenTitle = tr( "Open presets" );
            mIOSelection = tr( "Presets (*.bs *.BS)" );
            mSettingsRecent = "recentPresets";
            mDefaultSuffix = "bs";
            mPossibleSuffixes << "bs";
            mUpdateRecentOnSave = true;
        }
        break;

        default:
            qWarning() << "No type set";
    }
}

bool FileDealer::knows( const QString& suffix ) const {
    bool ok = mPossibleSuffixes.contains( suffix, Qt::CaseInsensitive );
    return ok;
}

QString FileDealer::getFilename() const {
    return mFilename;
}

void FileDealer::setFilename( const QString& filename ) {
    mFilename = filename;
    setCurrentFile( true );
}

std::pair<bool, QString> FileDealer::getSaveFilename() {

    // open filename ...
    QString dir = mFilename;

    QSettings settings;

    // ... or from settings
    if( dir.isEmpty() ) {
        dir = settings.value( mLastDirectory ).toString();
    }

    // ... or desktop
    if( dir.isEmpty() ) {
        dir = QDir::homePath();
    }

    QWidget* parent = mMenu ? mMenu->parentWidget() : 0;

    QString filename = QFileDialog::getSaveFileName( parent, mSaveTitle, dir, mIOSelection );

    if( filename.isEmpty() ) {
        return std::make_pair( false, QString() );
    }

    mFilename = filename;
    QFileInfo info( mFilename );

    if( ! mPossibleSuffixes.contains( info.suffix(), Qt::CaseInsensitive ) ) {
        mFilename += "." + mDefaultSuffix;
    }

    if( mUpdateRecentOnSave ) {
        setCurrentFile( false );
    }

    return std::make_pair( true, mFilename );
}

std::pair<bool, QString> FileDealer::getOpenFilename() {

    bool ok = true;

    QSettings settings;
    QString dir = settings.value( mLastDirectory ).toString();

    if( dir.isEmpty() ) {
        dir = QDir::homePath();
    }

    QWidget* parent = mMenu ? mMenu->parentWidget() : 0;

    QString filename = QFileDialog::getOpenFileName( parent, mOpenTitle, dir, mIOSelection );

    if( filename.isEmpty() ) {
        ok = false;
    } else {
        mFilename = filename;
        setCurrentFile( true );
    }

    return std::make_pair( ok, mFilename );
}

//! \brief Setup the menu under File -> Recent
//! \see void MainWindow::setupRecentMenu( )
void FileDealer::setupRecentMenu( QMenu* recent ) {

    mMenu = recent;

    for( int i = 0; i < MaxRecentFiles; ++i ) {
        mRecentFileActions[i] = new QAction( this );
        mRecentFileActions[i]->setVisible( false );
        connect( mRecentFileActions[i], SIGNAL( triggered() ), this, SLOT( openRecent() ) );
    }

    this->updateRecentFileActions();

    mMenu->addActions( mRecentFileActions );
}

//! \brief Update GUI state according to mFilename
//! \see void MainWindow::openImage( const QString& filename )
//! \arg open, true, if recent menu is updated from opening a file
void FileDealer::setCurrentFile( bool open ) {
    QSettings settings;

    /* set last opened directory */
    QFileInfo info( mFilename );
    QStringList files = settings.value( mSettingsRecent ).toStringList();

    if( !open || info.exists() ) {
        settings.setValue( mLastDirectory, info.absolutePath() );
        files.removeAll( mFilename );
        files.prepend( mFilename );
    }

    while( files.size() > MaxRecentFiles ) {
        files.removeLast();
    }

    settings.setValue( mSettingsRecent, files );

    this->updateRecentFileActions( open );
}

//! \brief Refreshs the Recent File Actions in menu and settings
//! \arg open, true, if recent menu is updated from opening a file
void FileDealer::updateRecentFileActions( bool open ) {
    QList<Qt::Key> numKeys;
    numKeys << Qt::Key_1
            << Qt::Key_2
            << Qt::Key_3
            << Qt::Key_4
            << Qt::Key_5;

    QSettings settings;
    QStringList files = settings.value( mSettingsRecent ).toStringList();

    int numRecentFiles = qMin( files.size(), ( int )MaxRecentFiles );

    int actionNo = 0;

    foreach( QAction* action, mRecentFileActions ) {
        action->setVisible( false );
    }

    for( int i = 0; i < numRecentFiles; ++i ) {
        QFileInfo info( files[i] );

        if( !open || info.exists() ) {
            mRecentFileActions[i]->setText( info.fileName() );
            mRecentFileActions[i]->setData( files[i] );
            mRecentFileActions[i]->setVisible( true );

            if( mType == ImagesType ) {
                mRecentFileActions[i]->setShortcut( Qt::ALT + numKeys[actionNo] );
            }

            actionNo++;
            mRecentFileActions[i]->setToolTip( info.absoluteFilePath() );
            mRecentFileActions[i]->setStatusTip( info.absoluteFilePath() );
        }
    }

    if( mMenu ) {
        if( actionNo == 0 ) {
            mMenu->setDisabled( true );
        } else {
            mMenu->setEnabled( true );
        }
    }
}

//! \brief Slot called by mRecentFileActions
void FileDealer::openRecent() {
    QAction* action = qobject_cast<QAction*>( sender() );

    if( action ) {
        QString filename;
        filename = action->data().toString();

        /* received by  */
        emit signalOpen( filename );
    }
}
