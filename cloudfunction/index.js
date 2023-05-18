const {Datastore} = require('@google-cloud/datastore');
/**
 * Background Cloud Function to be triggered by Pub/Sub.
 * This function is exported by index.js, and executed when
 * the trigger topic receives a message.
 *
 * @param {object} data The event payload.
 * @param {object} context The event metadata.
 */
exports.subscribe = function subscribe(data, context) {
  const pubsubMessage = data;
  const datastore = new Datastore({
    projectId: 'johansiot-199910'
  });
  const pooldata = JSON.parse(Buffer.from(pubsubMessage.data, 'base64').toString());
  pooldata.timestamp = new Date(pubsubMessage.attributes.published_at);

  const entity = {
    key: datastore.key(['pooldata', pubsubMessage.messageId]),
    data: pooldata
    };
  datastore.save(entity)
  	//.then(() => { console.log(entity) })
    .catch((err) => {
      console.error('ERROR:', err);
    });
};