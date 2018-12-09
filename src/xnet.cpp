#include "../include/xnet/http"
#include "../include/xlog"

#include <curl/curl.h>
#include <curl/easy.h>
#include <cstring>

xnet::http::http() noexcept :
	curl(curl_easy_init())
{
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
}

xnet::http::http(http && other) :
  curl(other.curl)
{
	other.curl = nullptr;
}

xnet::http::~http() noexcept
{
	if(curl != nullptr)
		curl_easy_cleanup(curl);
}

std::optional<std::vector<std::byte>> xnet::http::download(std::string const & url)
{
	return download(url, [](float){});
}

std::optional<std::vector<std::byte>> xnet::http::download(std::string const & url, std::function<void(float)> const & on_progress)
{
	buffer.clear();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	xlog::log("http", xlog::verbose) << "download '" << url << "'";

	if(on_progress) on_progress(0.0f);

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		xlog::log("http", xlog::error) << "download failed: " << curl_easy_strerror(res);
		return std::nullopt;
	}

	if(on_progress) on_progress(100.0f);

	return buffer;
}

std::size_t xnet::http::write_data(void * data, size_t size, size_t nmemb, void *stream)
{
	auto & http = *reinterpret_cast<struct http*>(stream);

	auto const start = http.buffer.size();
	auto const length = size * nmemb;
	http.buffer.resize(http.buffer.size() + length);
	memcpy(&http.buffer[start], data, length);
	return length;
}
