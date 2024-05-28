#include "error.h"

VulkanException::VulkanException(VkResult result, const std::string& msg) : 
    std::runtime_error(msg),
    m_result(result)
{
    error_message = std::string(std::runtime_error::what()) + std::string{ " : " } + std::to_string(result);
}

const char* VulkanException::what() const noexcept
{
    return error_message.c_str();
}
