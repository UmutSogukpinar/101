using fan_league.Entities;
using fan_league.Mappers;
using fan_league.Repositories.Users;
using fan_league.Requests.Users;
using fan_league.Responses.Common;
using fan_league.Responses.Users;
using Microsoft.AspNetCore.Identity;

namespace fan_league.Services.Users;

public class UserService(
    IUserRepository userRepository,
    IPasswordHasher<User> passwordHasher) : IUserService
{
    private readonly IUserRepository _userRepository = userRepository;
    private readonly IPasswordHasher<User> _passwordHasher = passwordHasher;

    public async Task<PagedResponse<UserResponse>> GetAllAsync(
        GetUsersRequest request,
        CancellationToken cancellationToken = default)
    {
        (IReadOnlyCollection<User> items, int totalCount) =
            await _userRepository.GetPagedAsync(
            request.PageNumber,
            request.PageSize,
            cancellationToken);

        int totalPages = (int)Math.Ceiling(totalCount / (double)request.PageSize);

        UserResponse[] users = items
            .Select(user => user.ToResponse())
            .ToArray();

        return new PagedResponse<UserResponse>(
            users,
            request.PageNumber,
            request.PageSize,
            totalCount,
            totalPages
            );
    }

    public async Task<UserResponse> GetByIdAsync(
        Guid id,
        CancellationToken cancellationToken = default)
    {
        return (await GetUserAsync(id, cancellationToken)).ToResponse();
    }

    public async Task<UserResponse> CreateAsync(
        CreateUserRequest request,
        CancellationToken cancellationToken = default)
    {
        User user = request.ToEntity();
        user.PasswordHash = _passwordHasher.HashPassword(user, request.Password);
        await _userRepository.AddAsync(user, cancellationToken);

        return user.ToResponse();
    }

    public async Task<UserResponse> UpdateAsync(
        Guid id,
        UpdateUserRequest request,
        CancellationToken cancellationToken = default)
    {
        User user = await GetUserAsync(id, cancellationToken);
        request.Apply(user);
        await _userRepository.UpdateAsync(user, cancellationToken);

        return user.ToResponse();
    }

    public async Task DeleteAsync(
        Guid id,
        CancellationToken cancellationToken = default)
    {
        if (!await _userRepository.DeleteAsync(id, cancellationToken))
        {
            throw new NotFoundException(nameof(User), id);
        }
    }

    private async Task<User> GetUserAsync(
        Guid id,
        CancellationToken cancellationToken)
    {
        return await _userRepository.GetByIdAsync(id, cancellationToken)
            ?? throw new NotFoundException(nameof(User), id);
    }
}
