const Datastore = require('@google-cloud/datastore');
/**
 * Background Cloud Function to be triggered by Pub/Sub.
 * This function is exported by index.js, and executed when
 * the trigger topic receives a message.
 *
 * @param {object} data The event payload.
 * @param {object} context The event metadata.
 */
exports.subscribe = function subscribe(data, context) {
  const pubsubMessage = data.data;
  const datastore = Datastore({
    projectId: 'poolbuddy-164819'
  });
  const waterdata = JSON.parse(Buffer.from(pubsubMessage.data, 'base64').toString());
  waterdata.timestamp = new Date(pubsubMessage.attributes.published_at);

  const entity = {
    key: datastore.key(['pooldata', event.eventId]),
    data: waterdata
    };

  datastore.save(entity)
  	//.then(() => { console.log(entity) })
    .catch((err) => {
      console.error('ERROR:', err);
    });
};
