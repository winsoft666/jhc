﻿#ifndef JHC_CONFIG_HPP__
#define JHC_CONFIG_HPP__
#pragma once

#undef JHC_NOT_HEADER_ONLY

#ifdef JHC_NOT_HEADER_ONLY
#define JHC_INLINE
#else
#define JHC_INLINE inline
#endif

#endif //!JHC_CONFIG_HPP__