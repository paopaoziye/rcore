#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_string.h>
#include <sbi_utils/fdt/fdt_domain.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/fdt/fdt_pmu.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/reset/fdt_reset.h>


extern struct sbi_platform platform;
static u32 my_board_hart_index2id[SBI_HARTMASK_MAX_BITS] = { 0 };

// 解析设备树并进行平台初始化
unsigned long fw_platform_init(unsigned long arg0, unsigned long arg1,
				unsigned long arg2, unsigned long arg3,
				unsigned long arg4)
{
	const char *model;
	void *fdt = (void *)arg1;
	u32 hartid, hart_count = 0;
	int rc, root_offset, cpus_offset, cpu_offset, len;
	//查找设备树的根节点
	root_offset = fdt_path_offset(fdt, "/");
	if (root_offset < 0)
		goto fail;
	// 读取平台名称
	model = fdt_getprop(fdt, root_offset, "model", &len);
	if (model)
		sbi_strncpy(platform.name, model, sizeof(platform.name));
	// 查找CPUS节点
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	if (cpus_offset < 0)
		goto fail;
	// 记录每个CPU的有效hart id以及hart数量
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		if (rc)
			continue;

		if (SBI_HARTMASK_MAX_BITS <= hartid)
			continue;

		my_board_hart_index2id[hart_count++] = hartid;
	}

	platform.hart_count = hart_count;

	return arg1;

fail:
	while (1)
		wfi();
}

// 早期初始化
static int my_board_early_init(bool cold_boot)
{

	return 0;
}
// 最终初始化
static int my_board_final_init(bool cold_boot)
{
	void *fdt;
    // 如果是冷启动，则重置设备树相关的硬件状态
	if (cold_boot)
		fdt_reset_init();
	if (!cold_boot)
		return 0;
    // 读取设备树指针并进行修复
	fdt = sbi_scratch_thishart_arg1_ptr();

	fdt_cpu_fixup(fdt);
	fdt_fixups(fdt);
	fdt_domain_fixup(fdt);

	return 0;
}
// 退出函数
static void my_board_early_exit(void)
{

}

static void my_board_final_exit(void)
{

}
// 初始化domains
static int my_board_domains_init(void)
{
	return fdt_domains_populate(fdt_get_address());
}
// PMU即电源管理单元初始化
static int my_board_pmu_init(void)
{
	return fdt_pmu_setup(fdt_get_address());
}
// 将对应的PMU事件转化为特定的 mhpmevent 寄存器值
static uint64_t my_board_pmu_xlate_to_mhpmevent(uint32_t event_idx,
					       uint64_t data)
{
	uint64_t evt_val = 0;

	// RAW事件，直接采用data
	if (event_idx == SBI_PMU_EVENT_RAW_IDX)
		evt_val = data;
	else {
		// 尝试从设备树获取对应事件的映射，若没有则采用event_idx
		evt_val = fdt_pmu_get_select_value(event_idx);
		if (!evt_val)
			evt_val = (uint64_t)event_idx;
	}

	return evt_val;
}
// 定义TLB范围刷新的最大限制值
static u64 my_board_tlbr_flush_limit(void)
{
	return SBI_PLATFORM_TLB_RANGE_FLUSH_LIMIT_DEFAULT;
}
// 平台相关的操作函数合集
const struct sbi_platform_operations platform_ops = {
	.early_init		= my_board_early_init,      
	.final_init		= my_board_final_init,            
	.early_exit		= my_board_early_exit,            
	.final_exit		= my_board_final_exit,            
	.domains_init	= my_board_domains_init,      
	.console_init	= fdt_serial_init,              
	.irqchip_init	= fdt_irqchip_init,             
	.irqchip_exit	= fdt_irqchip_exit,             
	.ipi_init		= fdt_ipi_init,
	.ipi_exit		= fdt_ipi_exit,
	.pmu_init		= my_board_pmu_init,              
	.pmu_xlate_to_mhpmevent = my_board_pmu_xlate_to_mhpmevent,
	.get_tlbr_flush_limit	= my_board_tlbr_flush_limit, 
	.timer_init		= fdt_timer_init,
	.timer_exit		= fdt_timer_exit,
};
// 平台的描述与配置信息
struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			    = "My-Board",
	.features			= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count			= SBI_HARTMASK_MAX_BITS,
	.hart_index2id		= my_board_hart_index2id,
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};