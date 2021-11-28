/**
 * Copyright (c) 2018 - 2018, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef NRF_CRYPTO_RNG_H__
#define NRF_CRYPTO_RNG_H__

/** @file
 *
 * @defgroup nrf_crypto_rng RNG related functions
 * @{
 * @ingroup nrf_crypto
 *
 * @brief RNG related functions
 *
 * @details There are two available RNG backends:
 *              - ARM CryptoCell CC310 (default for devices with CC310).
 *              - nRF HW RNG peripheral.
 *                * CTR-DRBG mode - nRF HW RNG used for seeding mbed TLS CTR-DRBG (default for
 *                  devices without CC310).
 *                * Raw mode - all data is generated by the nRF HW RNG.
 *
 *          The CC310 backend meets the standards NIST 800-90B3 and AIS-31 (Class “P2 High”), and
 *          should be preferred in most cases on devices that includes the CC310 core. Devices that
 *          do not include CC310 should normally use the nRF HW RNG with mbed TLS CTR-DRBG. The
 *          mbed TLS CTR-DRBG code is standardized by NIST (SP 800-90A Rev. 1).
 */

#include "sdk_common.h"
#include "nrf_crypto_error.h"
#include "nrf_crypto_rng_shared.h"
#include "nrf_crypto_rng_backend.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Context type for RNG.
 *
 * @note The actual type depend on the backend in use.
 */
typedef nrf_crypto_backend_rng_context_t nrf_crypto_rng_context_t;


/**
 * @brief Temporary work buffer type for RNG.
 *
 * @details Only needed during initializing. Can be freed when @ref nrf_crypto_rng_init has
 *          returned. Not needed if @ref NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED is enabled
 *          in @ref sdk_config.
 *
 * @note The actual type depend on the backend in use.
 */
typedef nrf_crypto_backend_rng_temp_buffer_t nrf_crypto_rng_temp_buffer_t;


/**@brief Initialize the random number generator.
 *
 * @details This function has no effect when @ref NRF_CRYPTO_RNG_AUTO_INIT_ENABLED is enabled.
 *
 * @warning The p_temp_buffer is 6112 bytes when using the CC310 backend. Ensure that stack size
 *          is sufficient if allocated on stack. Applications that use nRF HW RNG as backend or are
 *          not RAM constrained can use internal static allocation of context and temporary buffers
 *          (@ref NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED).
 *
 * @note  The context object can be reused without the need for a full reinitialization of the
 *        backend in case of for example wakeup from system OFF, provided that the context is
 *        located in a memory block that is retained. This only apply to the CC310 backend, and when
 *        the context is allocated manually (NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED disabled).
 *
 * @param[in]       p_context       Pointer to context memory. The context will be managed
 *                                  internally, and the pointer is not used for subsequent calls to
 *                                  the nrf_crypto_rng API. The context memory is needed until
 *                                  @ref nrf_crypto_rng_uninit is called, so it should normally not
 *                                  be on the stack. Use NULL if
 *                                  @ref NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED is enabled
 *                                  in @ref sdk_config (recommended for most applications).
 *
 * @param[in,out]   p_temp_buffer   Temporary buffer needed during initialization of the backend. It
 *                                  is not used after the return of this function, and can be freed
 *                                  at that point. Buffer is allocated internally if the pointer is
 *                                  NULL, using the allocated defined by @ref NRF_CRYPTO_ALLOCATOR
 *                                  in @c sdk_config.h. Use NULL if
 *                                  @ref NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED is enabled
 *                                  in @ref sdk_config (recommended for most applications).
 *
 * @retval  NRF_SUCCESS                         If random number generator was initialized
 *                                              successfully.
 * @retval  NRF_ERROR_CRYPTO_NOT_INITIALIZED    @ref nrf_crypto_init was not called prior to this
 *                                              function.
 * @retval  NRF_ERROR_CRYPTO_CONTEXT_NULL       p_context was NULL.
 * @retval  NRF_ERROR_CRYPTO_INTERNAL           If an internal error occurred in the nrf_crypto
 *                                              backend.
 * @retval  NRF_ERROR_CRYPTO_ALLOC_FAILED       Unable to allocate memory for the context or work
 *                                              buffer.
 * @retval  NRF_ERROR_CRYPTO_STACK_OVERFLOW     Stack overflow detected. Typically caused by
 *                                              allocating an instance of
 *                                              @ref nrf_crypto_rng_temp_buffer_t
 *                                              on the stack when using CC310 backend.
 * @retval  NRF_ERROR_CRYPTO_BUSY               RNG is busy. Rerun at a later time.
 */
ret_code_t nrf_crypto_rng_init(nrf_crypto_rng_context_t       * p_context,
                               nrf_crypto_rng_temp_buffer_t   * p_temp_buffer);


/**@brief Uninitialize the random number generator.
 *
 * @retval  NRF_SUCCESS                                 If RNG was uninitialized successfully.
 * @retval  NRF_ERROR_CRYPTO_CONTEXT_NOT_INITIALIZED    RNG has not been initialized.
 * @retval  NRF_ERROR_CRYPTO_INTERNAL                   If an internal error occurred in the
 *                                                      nrf_crypto backend.
 * @retval  NRF_ERROR_CRYPTO_BUSY                       RNG is busy. Rerun at a later time.
 */
ret_code_t nrf_crypto_rng_uninit(void);


/**@brief Generate random data of given size.
 *
 * @details @ref nrf_crypto_rng_init must be called prior to this function unless
 *          @ref NRF_CRYPTO_RNG_AUTO_INIT_ENABLED is enabled in @ref sdk_config.
 *
 * @param[in,out]   p_target    Buffer to hold the random generated data.
 *                              This buffer must be at least as large as the size parameter.
 * @param[in]       size        Length (in bytes) to generate random data for.
 *
 * @retval  NRF_SUCCESS                                 Data was generated successfully.
 * @retval  NRF_ERROR_CRYPTO_NOT_INITIALIZED            @ref nrf_crypto_init was not called prior to
 *                                                      this function.
 * @retval  NRF_ERROR_CRYPTO_CONTEXT_NOT_INITIALIZED    @ref nrf_crypto_rng_init was not called
 *                                                      prior to this function and
 *                                                      @ref NRF_CRYPTO_RNG_AUTO_INIT_ENABLED is
 *                                                      disabled.
 * @retval  NRF_ERROR_CRYPTO_OUTPUT_NULL                p_target was NULL.
 * @retval  NRF_ERROR_CRYPTO_OUTPUT_LENGTH              Size was 0 or larger than the backend
 *                                                      supports.
 * @retval  NRF_ERROR_CRYPTO_INTERNAL                   If an internal error occurred in the
 *                                                      backend.
 * @retval  NRF_ERROR_CRYPTO_STACK_OVERFLOW             Stack overflow detected in
 *                                                      @ref nrf_crypto_rng_init when using auto
 *                                                      initialization. Typically caused by
 *                                                      allocating an instance of
 *                                                      @ref nrf_crypto_rng_temp_buffer_t
 *                                                      on the stack when using CC310 backend.
 * @retval  NRF_ERROR_CRYPTO_BUSY                       RNG is busy. Rerun at a later time.
 */
ret_code_t nrf_crypto_rng_vector_generate(uint8_t * const p_target, size_t size);


/**@brief Generate a vector of constrained random data of given size, between the specified min
 *        and max values.
 *
 * @details @ref nrf_crypto_rng_init must be called prior to this function unless
 *          @ref NRF_CRYPTO_RNG_AUTO_INIT_ENABLED is enabled in @ref sdk_config.
 *
 *          All vectors are in big-endian format, with the most significant byte as the first
 *          element / lowest address.
 *
 * @note This function may execute for a long time if the window between p_min and p_max is small.
 *
 * @param[in,out]   p_target    Buffer to hold the random generated data.
 *                              This buffer must be at least as large as the size parameter.
 * @param[in]       p_min       Byte array defining the lower limit of the random vector.
 * @param[in]       p_max       Byte array defining the upper limit of the random vector.
 * @param[in]       size        Length (in bytes) to generate random data for. Note that all three
 *                              buffers (p_target, p_min and p_max) must be of this size.
 *
 * @retval  NRF_SUCCESS                                 Data was generated successfully.
 * @retval  NRF_ERROR_CRYPTO_NOT_INITIALIZED            @ref nrf_crypto_init was not called prior to
 *                                                      this function.
 * @retval  NRF_ERROR_CRYPTO_CONTEXT_NOT_INITIALIZED    @ref nrf_crypto_rng_init was not called
 *                                                      prior to this function and
 *                                                      @ref NRF_CRYPTO_RNG_AUTO_INIT_ENABLED is
 *                                                      disabled.
 * @retval  NRF_ERROR_CRYPTO_OUTPUT_NULL                p_target was NULL.
 * @retval  NRF_ERROR_CRYPTO_INPUT_NULL                 p_min or p_max was NULL.
 * @retval  NRF_ERROR_CRYPTO_OUTPUT_LENGTH              Size was 0 or larger than the backend
 *                                                      supports.
 * @retval  NRF_ERROR_CRYPTO_INTERNAL                   If an internal error occurred in the
 *                                                      backend.
 * @retval  NRF_ERROR_CRYPTO_STACK_OVERFLOW             Stack overflow detected in
 *                                                      @ref nrf_crypto_rng_init when using auto
 *                                                      initialization. Typically caused by
 *                                                      allocating an instance of
 *                                                      @ref nrf_crypto_rng_temp_buffer_t
 *                                                      on the stack when using CC310 backend.
 * @retval  NRF_ERROR_CRYPTO_BUSY                       RNG is busy. Rerun at a later time.
 */
ret_code_t nrf_crypto_rng_vector_generate_in_range(uint8_t          * const p_target,
                                                   uint8_t    const * const p_min,
                                                   uint8_t    const * const p_max,
                                                   size_t                   size);


/**
 * @brief This function is used for reseeding the RNG with additional entropy.
 * 
 * @details The backends will reseed automatically when required. This function can be used to
 *          reseed at specific times and to provide additional data that is used to add personalized
 *          randomness.
 *
 * @note Reseeding is not supported if using the nRF HW RNG backend without mbed TLS CTR-DRBG
 *       (NRF_CRYPTO_BACKEND_NRF_HW_RNG_MBEDTLS_CTR_DRBG_ENABLED disabled in sdk_config.h).
 *
 * @warning The p_temp_buffer is 6112 bytes when the CC310 backend is used. Ensure that stack size
 *          is sufficient if allocated on stack.
 *
 * @param[in,out]   p_temp_buffer   Temporary buffer needed during reseeding. It
 *                                  is not used after the return of this function, and can be freed
 *                                  at that point. Buffer is allocated internally if the pointer is
 *                                  NULL, using the allocated defined by @ref NRF_CRYPTO_ALLOCATOR
 *                                  in @c sdk_config.h. Use NULL if
 *                                  @ref NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED is enabled
 *                                  in @ref sdk_config (recommended for most applications).
 * @param[in]       p_input_data    Optional input data used to increase the entropy.
 * @param[in]       size            Length of input data. Must be 0, 4, 8 or 12 for CC310.
 *
 * @retval  NRF_SUCCESS                                 Data was generated successfully.
 * @retval  NRF_ERROR_CRYPTO_NOT_INITIALIZED            @ref nrf_crypto_init was not called prior to
 *                                                      this function.
 * @retval  NRF_ERROR_CRYPTO_CONTEXT_NOT_INITIALIZED    @ref nrf_crypto_rng_init was not called
 *                                                      prior to this function and
 *                                                      @ref NRF_CRYPTO_RNG_AUTO_INIT_ENABLED is
 *                                                      disabled.
 * @retval  NRF_ERROR_CRYPTO_INPUT_NULL                 p_temp_buffer was NULL or p_input_data was
 *                                                      NULL and size > 0 .
 * @retval  NRF_ERROR_CRYPTO_INPUT_LENGTH               Invalid input data size.
 * @retval  NRF_ERROR_CRYPTO_FEATURE_UNAVAILABLE        Reseeding not supported by backend.
 * @retval  NRF_ERROR_CRYPTO_INTERNAL                   If an internal error occurred in the
 *                                                      backend.
 * @retval  NRF_ERROR_CRYPTO_STACK_OVERFLOW             Stack overflow detected. Typically caused by
 *                                                      allocating an instance of
 *                                                      @ref nrf_crypto_rng_temp_buffer_t
 *                                                      on the stack when using CC310 backend.
 * @retval  NRF_ERROR_CRYPTO_BUSY                       RNG is busy. Rerun at a later time.
 */
ret_code_t nrf_crypto_rng_reseed(nrf_crypto_rng_temp_buffer_t * p_temp_buffer,
                                 uint8_t                      * p_input_data,
                                 size_t                         size);


#ifdef __cplusplus
}
#endif

/**@} */

#endif // NRF_CRYPTO_RNG_H__
