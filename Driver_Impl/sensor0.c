#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/device.h>

#define DEVICE_NAME "sensor0"
#define CLASS_NAME  "vsensor"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luccas");
MODULE_DESCRIPTION("Character device LKM que simula um sensor virtual gerando dados aleatórios.");
MODULE_VERSION("1.0.0");

static int    majorNumber;
static struct class*  sensorClass  = NULL;
static struct device* sensorDevice = NULL;
static char   message[128] = {0};
static short  message_size;
static int    numberOpens = 0;

// Prototypes for file operations
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// File operations struct
static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

// Função chamada quando o dispositivo é aberto
static int dev_open(struct inode *inodep, struct file *filep){
    numberOpens++;
    printk(KERN_INFO "sensor0: Dispositivo aberto %d vez(es).\n", numberOpens);
    return 0;
}

// Função chamada quando o dispositivo é lido
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    u32 random_number;
    int error_count = 0;
    char sensor_data[32];

    get_random_bytes(&random_number, sizeof(random_number));
    snprintf(sensor_data, sizeof(sensor_data), "Sensor: %u\n", random_number);

    message_size = strlen(sensor_data);

    error_count = copy_to_user(buffer, sensor_data, message_size);

    if (error_count == 0){
        printk(KERN_INFO "sensor0: Enviados %d bytes ao usuário\n", message_size);
        return (message_size); // Retorna número de bytes lidos
    } else {
        printk(KERN_WARNING "sensor0: Falha ao enviar %d bytes ao usuário\n", error_count);
        return -EFAULT; // Falha ao copiar para espaço do usuário
    }
}

// Função chamada quando o dispositivo é escrito
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    size_t to_copy = len > sizeof(message)-1 ? sizeof(message)-1 : len;
    if(copy_from_user(message, buffer, to_copy) != 0) {
        printk(KERN_WARNING "sensor0: Falha ao receber dados do usuário.\n");
        return -EFAULT;
    }
    message[to_copy] = '\0';
    printk(KERN_INFO "sensor0: Mensagem recebida do usuário: %s\n", message);
    return to_copy;
}

// Função chamada ao liberar o dispositivo
static int dev_release(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "sensor0: Dispositivo fechado.\n");
    return 0;
}

// Função de inicialização do módulo
static int __init sensor_init(void){
    printk(KERN_INFO "sensor0: Inicializando o sensor virtual (character device)...\n");

    // Registra um número de dispositivo maior dinamicamente
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
        printk(KERN_ALERT "sensor0: Falha ao registrar um major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "sensor0: Registrado corretamente com major number %d\n", majorNumber);

    // Registrar a classe do dispositivo
    sensorClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(sensorClass)){
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "sensor0: Falha ao registrar a classe do dispositivo\n");
        return PTR_ERR(sensorClass);
    }
    printk(KERN_INFO "sensor0: Classe do dispositivo registrada corretamente\n");

    // Registrar o próprio dispositivo
    sensorDevice = device_create(sensorClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(sensorDevice)){
        class_destroy(sensorClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "sensor0: Falha ao criar o dispositivo\n");
        return PTR_ERR(sensorDevice);
    }
    printk(KERN_INFO "sensor0: Dispositivo criado corretamente\n");
    return 0;
}

// Função de saída do módulo
static void __exit sensor_exit(void){
    device_destroy(sensorClass, MKDEV(majorNumber, 0));
    class_unregister(sensorClass);
    class_destroy(sensorClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "sensor0: Módulo removido!\n");
}

module_init(sensor_init);
module_exit(sensor_exit);