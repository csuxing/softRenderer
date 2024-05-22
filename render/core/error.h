#ifndef _ERROR_H_H_
#define _ERROR_H_H_

#include <stdexcept>
#include <string>
#include <volk.h>

class VulkanException : public std::runtime_error
{
public:
	/**
	 * @brief Vulkan exception constructor
	 */
	VulkanException(VkResult result, const std::string& msg = "Vulkan error");

	/**
	 * @brief Returns the Vulkan error code as string
	 * @return String message of exception
	 */
	const char* what() const noexcept override;

	VkResult m_result;

private:
	std::string error_message;
};

#define VK_CHECK(x)         \
    {                       \
        VkResult err = x;   \
        if (err)            \
        {                   \
            abort();        \
        }                   \
    }
    

#endif
