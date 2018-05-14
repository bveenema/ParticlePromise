
/* Singleton or multiple objects to manage promises? Singleton
      multiple objects would require multiple Particle.subscribe()'s

Webhook must be formatted as:
  Response Topic:
    promise-response/success/{webhookName}
  Error Response Topic:
    promise-response/error/{webhookName}

Software Timer vs .updaate()?
*/



void successFunction(const char* event, const char* data); // function to run on success
void errorFunction(const char* event, const char* data); // function to run on error
void timeoutFunction(); // function to run on timeout
void finalFunction(); // function to run after success, error or timeout function.  ie - no matter what happened, do this
void sendWebhookFunction(); // function to create the webhook call (needs to call Particle.publish);

ParticlePromise<{bufferSize}, {maxTopicLength}> promise;  // Creates a buffer for function pointers and response topics. And calls Particle.subscribe() with filter "promise-response"

void promise.setTimeout(int); // Sets the default timeout for response, if not set, default timeout is 5 seconds.

void promise.create(*sendWebhookFunction, "myResponseTopic", *successFunction, {*errorFunction}, {*timeoutFunction}, {int timeout}, {*finalFunction});
auto index = promise.create(*sendWebhookFunction, "myResonseTopic");
                index.then(*successFunction);
                index.catch(*errorFunction);
                index.timeout(*timeoutFunction, {int timeout});
                index.finally(*finalFunction);

promise.reEnable(); // Re-creates Particle.subscribe() in case of a previous Particle.unsubscribe() call
