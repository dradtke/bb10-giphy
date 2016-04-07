/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/ImageView>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/TextField>
using namespace bb::cascades;

#include <bb/data/JsonDataAccess>
using namespace bb::data;

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Network access.
    network = new QNetworkAccessManager(this);

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    TextField *searchField = root->findChild<TextField*>("searchField");
    res = QObject::connect(searchField->input(),
            SIGNAL(submitted(bb::cascades::AbstractTextControl*)),
            this,
            SLOT(onSearch(bb::cascades::AbstractTextControl*)));

    Q_ASSERT(res);

    // Locate components.
    loading = root->findChild<ActivityIndicator*>("loading");
    image = root->findChild<ImageView*>("theImage");

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

/*
 * User submitted something in the search field.
 */
void ApplicationUI::onSearch(bb::cascades::AbstractTextControl *control)
{
    qDebug() << "Searching for: " << control->text();
    loading->setVisible(true);
    loading->start();
    image->setVisible(false);

    // Create a network request.
    QUrl url = QUrl("http://api.giphy.com/v1/gifs/random");
    QList< QPair<QString, QString> > params;
    params.append(qMakePair(QString("api_key"), QString("dc6zaTOxFJmzC")));
    params.append(qMakePair(QString("tag"), QString(control->text())));
    url.setQueryItems(params);
    qDebug() << "Sending query to: " << url.toString();

    QNetworkRequest req = QNetworkRequest();
    req.setUrl(url);

    QNetworkReply *reply = network->get(req);
    if (reply == NULL) {
        qDebug() << "network->get(req) returned NULL!";
    } else {
        bool res;
        Q_UNUSED(res);

        res = QObject::connect(reply,
                    SIGNAL(finished()),
                    this,
                    SLOT(onSearchRequestFinished()));

        Q_ASSERT(res);
    }
}

void ApplicationUI::onSearchRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    // Parse the response.
    JsonDataAccess jda;
    QVariant ob = jda.load(reply);
    QVariantMap root = ob.value<QVariantMap>();
    QVariantMap data = root.value("data").value<QVariantMap>();
    QString url = data.value("image_url").toString();
    qDebug() << "Downloading image: " << url;

    // Download the image.
    QNetworkRequest req = QNetworkRequest();
    req.setUrl(QUrl(url));

    QNetworkReply *imageReply = network->get(req);
    if (imageReply == NULL) {
        qDebug() << "network->get(req) returned NULL!";
    } else {
        bool res;
        Q_UNUSED(res);

        res = QObject::connect(imageReply,
                    SIGNAL(finished()),
                    this,
                    SLOT(onDownloadRequestFinished()));

        Q_ASSERT(res);
    }
}

void ApplicationUI::onDownloadRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    loading->stop();
    if (reply->error() == QNetworkReply::NoError) {
        image->setVisible(true);
        image->setImage(Image(reply->readAll()));
    } else {
        qDebug() << "Image failed to download: " << reply->error();
    }
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Giphy_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}
