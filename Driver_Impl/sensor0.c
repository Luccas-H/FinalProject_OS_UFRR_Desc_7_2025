#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/device.h>
#include <linux/version.h> 

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


static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


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
    int error_count = 0;

  
    message_size = strlen(message);  

    if (*offset >= message_size) {
        return 0;  
    }

    
    if (len > message_size - *offset) {
        len = message_size - *offset;
    }

 
    error_count = copy_to_user(buffer, message + *offset, len);

    if (error_count == 0){
        *offset += len;  
        printk(KERN_INFO "sensor0: Enviados %zu bytes ao usuário\n", len);
        return len;  
    } else {
        printk(KERN_WARNING "sensor0: Falha ao enviar dados ao usuário\n");
        return -EFAULT;
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
    if (majorNumber < 0){
        printk(KERN_ALERT "sensor0: Falha ao registrar um major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "sensor0: Registrado corretamente com major number %d\n", majorNumber);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    sensorClass = class_create(CLASS_NAME);
#else
    sensorClass = class_create(THIS_MODULE, CLASS_NAME);
#endif

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

