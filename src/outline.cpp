









void sucessFunction(const char* event, const char* data); // function to run on success
void errorFunction(const char* event, const char* data); // function to run on error
void timeoutFunction(); // function to run on timeout
void sendWebhookFunction(); // function to create the webhook call (needs to call Particle.publish);

MyPromise getForecast(*successFunction, *errorFunction, *timeoutFunction); // create promise object

getForecast.createSubscribe("myResonseTopic"); //creates a Particle.subscribe that listens for the response

getForecast.promise(*sendWebhookFunction); // send the webhook
