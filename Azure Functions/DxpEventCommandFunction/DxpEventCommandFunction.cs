using System.Configuration;
using System.Threading.Tasks;
using Microsoft.Azure.Devices;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Host;
using Microsoft.ServiceBus.Messaging;
using Newtonsoft.Json;

namespace DxpEventCommandFunction
{
    public static class DxpEventCommandFunction
    {
        private static ServiceClient _client;
        
        [FunctionName("DxpEventCommandFunction")]
        public static async Task Run([ServiceBusTrigger("iothubcommandqueue", AccessRights.Manage, Connection = "ServiceBusQueueConnection")]string myQueueItem, TraceWriter log)
        {
            log.Info($"Function is triggered by device");

            var command = JsonConvert.DeserializeObject<IotHubCommand>(myQueueItem);
            
            _client = ServiceClient.CreateFromConnectionString(ConfigurationManager.AppSettings["IotHubConnectionString"]);
            var method = new CloudToDeviceMethod(command.Command);
            var result = await _client.InvokeDeviceMethodAsync(command.DeviceId, method);
        }
    }

    public class IotHubCommand
    {
        public string Command { get; set; }
        public string DeviceId { get; set; }
    }
}
