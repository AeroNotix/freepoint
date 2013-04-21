#include <QtNetwork/QNetworkRequest>
#include "requests.h"

void SetHeaders(QNetworkRequest & req) {
    req.setRawHeader("Accept", "application/json");
	req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
}
